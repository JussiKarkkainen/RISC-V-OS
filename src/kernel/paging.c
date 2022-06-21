#include <stdint.h>
#include "paging.h"
#include "../libc/include/stdio.h"
#include "../libc/include/string.h"
#include "process.h"
#include "syscall.h" 

uint32_t *kpagetable;

extern char uvec[];

extern char text_end[];

static inline void flush_tlb(void) {
    asm volatile("sfence.vma zero, zero");
}

// Write kernel pagetable root address to satp and set mmu to sv32
static inline void satp_write(uint32_t kpagetable) {
    asm volatile("csrw satp, %0" : : "r" ((1L << 31) | ((uint32_t)kpagetable >> 12)));

}

// create the kernel pagetable
uint32_t *kpagemake(void) {
    uint32_t *kpage = kalloc(1);
    memset(kpage, 0, PGESIZE);

    // Create a virtual memory map
    kmap(kpage, UART0, UART0, PGESIZE, PTE_R | PTE_W);
   /* 
    kprintf("UART0 %p\nVIRTIO %p\nPLIC %p\nKERNEL_BASE %p\nTEXT_END %p\nUSERVEC %p\nMAXVA %p\n", 
            UART0, VIRTIO0, PLIC, KERNEL_BASE, (uint32_t)text_end, (uint32_t)uvec, MAXVA);
    */
    kmap(kpage, VIRTIO0, VIRTIO0, PGESIZE, PTE_R | PTE_W);
    
    kmap(kpage, CLINT, CLINT, 0x10000, PTE_R | PTE_W);

    kmap(kpage, PLIC, PLIC, PLICSIZE, PTE_R | PTE_W);
    
    kmap(kpage, KERNEL_BASE, KERNEL_BASE, (uint32_t)text_end-KERNEL_BASE, PTE_R | PTE_X);
    
    kmap(kpage, (uint32_t)text_end, (uint32_t)text_end, PHYSTOP-(uint32_t)text_end, PTE_R | PTE_W);
     
    kmap(kpage, USERVEC, (uint32_t)uvec, PGESIZE, PTE_R | PTE_X);
    
    map_kstack(kpage);

    return kpage;
}

// Initializes the kernel page table
void kpage_init(void) {
    kpagetable = kpagemake();
}

// Write address of kernel/root pagetable to satp reg and flush TLBs
void init_paging(void) {
    satp_write((uint32_t)kpagetable);
    flush_tlb();
}

// Finds the PTE for a virtual address
uint32_t *walk(uint32_t *pagetable, uint32_t vir_addr, int alloc) {
    
    if (vir_addr >= MAXVA) {
        panic("vir_addr out of range");
    }
    
    for (int i = 1; i > 0; i--) {

        uint32_t *pte = &pagetable[((vir_addr >> (PGEOFFSET + (10 * i))) & VPNMASK)];
        
        // Turn pte into phy_addr
        if (*pte & PTE_V) {
            // Shift PPNs to correct places from pte
            pagetable = (uint32_t *)((*pte >> 10) << 12);
        }
        // Turn phy_addr into pte
        else {
            if (!alloc || ((pagetable = kalloc(1)) == 0)) {
                return 0;
            }
            *pte = (((uint32_t)pagetable >> 12) << 10) | PTE_V;
        }
    }
    return &pagetable[((vir_addr >> (PGEOFFSET + (10 * 0))) & VPNMASK)];
}   

uint32_t fetch_pa_addr(uint32_t *pagetable, uint32_t va) {
    uint32_t *pte;
    uint32_t pa;
    
    if (va >= MAXVA) {
        return 0;
    }

    pte = walk(pagetable, va, 0);
    if (pte == 0) {
        return 0;
    }
    if ((*pte & PTE_V) == 0) {
        return 0;
    }
    if ((*pte & PTE_U) == 0) {
        return 0;
    }
    pa = ((*pte >> 10) << 12);
    return pa;
}

uint32_t fetch_kpa(uint32_t va) {
    
    uint32_t off = va % PGESIZE;
    uint32_t *pte;
    uint32_t pa;

    pte = walk(kpagetable, va, 0);
    if (pte == 0) {
        panic("pte = 0, fetch_kpa");
    }
    if ((*pte && PTE_V) == 0) {
        panic("PTE_V == 0, fetch_kpa");
    }
    pa = ((*pte << 10) << 12);
    return pa + off;
}

// Install PTEs for new mappings
int kmap(uint32_t *kpage, uint32_t vir_addr, uint32_t phy_addr, uint32_t size, int permissions) {

    uint32_t *pte;
    uint32_t last;
    uint32_t vir;
    
    if (size == 0) {
        panic("kmap: size == 0!");
    }
    last = ((vir_addr + size - 1) & ~(PGESIZE - 1));
    vir = (vir_addr & ~(PGESIZE - 1));
    
    while(1) {
        
        if ((pte = walk(kpage, vir, 1)) == 0) { 
            return -1;
        }
        if (*pte & PTE_V) {
            panic("kmap, PTE_V and pte");
        }

        *pte = ((phy_addr >> 12) << 10) | permissions | PTE_V;
        if (vir == last) {
            break;
        }
        vir += PGESIZE;
        phy_addr += PGESIZE;
    }
    return 0;
}

uint32_t *upaging_create(void) {
    uint32_t *pagetable;
    pagetable = kalloc(1);
    if (pagetable == 0) {
        return 0;
    }
    memset(pagetable, 0, PGESIZE);
    return pagetable;
}

// Load the user initcode into address 0 of pagetable for the first process.
void upaging_init(uint32_t *pagetable, unsigned char *src, unsigned int size) {
    uint32_t *mem;
    
    if (size >= PGESIZE) {
        panic("size >= PGESIZE, upaging_init()");
    }

    mem = kalloc(1);
    memset(mem, 0, PGESIZE);
    kmap(pagetable, 0, (uint32_t)mem, PGESIZE, PTE_W | PTE_R | PTE_X | PTE_U);

    memmove(mem, src, size);
}

// Allocate user pages for processes to grow process memory from oldsize to newsize
uint32_t uvmalloc(uint32_t *pagetable, uint32_t oldsize, uint32_t newsize) {

    uint32_t *mem;
    uint32_t a;

    if (newsize < oldsize) {
        return oldsize;
    }

    oldsize = (((oldsize) + PGESIZE-1) & ~(PGESIZE-1));      // Round up to pagesize
    a = oldsize;
    for (; a < newsize; a += PGESIZE) {
        mem = kalloc(1);
        if (mem == 0) {
            uvmdealloc(pagetable, a, oldsize);
            return 0;
        }
        memset(mem, 0, PGESIZE);
        if (kmap(pagetable, a, (uint32_t)mem, PGESIZE, PTE_W|PTE_X|PTE_R|PTE_U) != 0) {
            kfree(mem);
            uvmdealloc(pagetable, a, oldsize);
            return 0;
        }
    }
    return newsize;
}

// Deallocate user pages for processes to grow process memory from oldsize to newsize
uint32_t uvmdealloc(uint32_t *pagetable, uint32_t oldsize, uint32_t newsize) {

    if (newsize >= oldsize) {
        return oldsize;
    }

    uint32_t rounded_newsize = (((newsize) + PGESIZE-1) & ~(PGESIZE-1));      // Round up to pagesize
    uint32_t rounded_oldsize = (((oldsize) + PGESIZE-1) & ~(PGESIZE-1));    

    if (rounded_newsize < rounded_oldsize) {
        int num_pages = (rounded_oldsize - rounded_newsize) / PGESIZE;
        uvmunmap(pagetable, rounded_newsize, num_pages, 1);
    }
    return newsize;
}

void uvmclear(uint32_t *pagetable, uint32_t va) {
  
    uint32_t *pte;
  
    pte = walk(pagetable, va, 0);
    if (pte == 0) {
        panic("uvmclear");
    }
    *pte &= ~PTE_U;
}

int uvmcopy(uint32_t *old, uint32_t *new_addr, uint32_t size) {

    uint32_t *pte;
    uint32_t pa, i;
    unsigned int flags;
    uint32_t *mem;

    for (i = 0; i < size; i += PGESIZE) {
        if ((pte = walk(old, i, 0)) == 0) {
            panic("uvmcopy: pte should exist");
        }
        if ((*pte & PTE_V) == 0) {
            panic("uvmcopy: page not present");
        }
        pa = (((*pte) >> 10) << 12);
        flags = ((*pte) & 0x3ff);
        if ((mem = kalloc(1)) == 0) {
            goto err;
        }
        memmove(mem, (char*)pa, PGESIZE);
        if (kmap(new_addr, i, PGESIZE, (uint32_t)mem, flags) != 0) {
            kfree(mem);
            goto err;
        }
    }
    return 0;

    err:
        uvmunmap(new_addr, 0, i / PGESIZE, 1);
        return -1;
}

void uvmunmap(uint32_t *pagetable, uint32_t va, uint32_t num_pages, int free) {
  
    uint32_t a;
    uint32_t *pte;

    if ((va % PGESIZE) != 0) {
        panic("uvmunmap: not aligned");
    }

    for (a = va; a < va + num_pages*PGESIZE; a += PGESIZE) {
        if ((pte = walk(pagetable, a, 0)) == 0) {
            panic("uvmunmap: walk");
        }
        if ((*pte & PTE_V) == 0) {
            panic("uvmunmap: not mapped");
        }
        if ((*pte & 0x3ff) == PTE_V) {
            panic("uvmunmap: not a leaf");
        }
        if (free) {
            uint32_t pa = (((*pte) >> 10) << 12);
            kfree((void*)pa);
        }
        *pte = 0;
    }
}

void freewalk(uint32_t *pagetable) {

    for (int i = 0; i < 512; i++) {
        uint32_t pte = pagetable[i];
        if ((pte & PTE_V) && (pte & (PTE_R|PTE_W|PTE_X)) == 0) {
            uint32_t child = ((pte >> 10) << 12);
            freewalk((uint32_t *)child);
            pagetable[i] = 0;
        }
        else if (pte & PTE_V) {
            panic("freewalk, leaf"); 
        }
    }
    kfree((void *)pagetable);
}

void uvmfree(uint32_t *pagetable, uint32_t size) {

    if (size > 0) {
        uvmunmap(pagetable, 0, (((size) + PGESIZE-1) & ~(PGESIZE-1)) / PGESIZE, 1);
    }
    freewalk(pagetable);
} 

// Copies len amount of bytes to dst from virtual address srcaddr
// in a given pagetable. Returns 0 on success and -1 for failure
int copyto(uint32_t *pagetable, char *dst, uint32_t srcaddr, uint32_t len) {
    uint32_t n, va, pa;

    while (len > 0) {
        va = (srcaddr & ~(PGESIZE - 1));
        pa = fetch_pa_addr(pagetable, va);
        if (pa == 0) {
            return -1;
        }
        n = PGESIZE - (srcaddr - va);
        if (n > len) {
            n = len;
        }
        memmove(dst, (void *)(pa + (srcaddr - va)), n);

        len -= n;
        dst += n;
        srcaddr = va + PGESIZE;
    }
    return 0;
}

// Copy len amount of bytes to dstaddr from src in a given pagetable
// returns 0 on success and -1 for failure
int copyout(uint32_t *pagetable, uint32_t dstaddr, char *src, uint32_t len) {
    uint32_t n, va, pa;

    while(len > 0) {
        va = (dstaddr & ~(PGESIZE - 1));
        pa = fetch_pa_addr(pagetable, va);
        if (pa == 0) {
            return -1;
        }
        n = PGESIZE - (dstaddr - va);
        if (n > len) {
            n = len;
        }
        memmove((void *)(pa + (dstaddr - va)), src, n);

        len -= n;
        src += n;
        dstaddr = va + PGESIZE;
    }
    return 0;
}

// Copy a null-terminated string from user to kernel
int copyinstr(uint32_t *pagetable, char *dst, uint32_t srcva, uint32_t max) {
	uint32_t n, va0, pa0;
  	int got_null = 0;

  	while (got_null == 0 && max > 0) {
    	va0 = (srcva & ~(PGESIZE - 1));
    	pa0 = fetch_pa_addr(pagetable, va0);
    	
		if (pa0 == 0) {
      		return -1;
		}
    	n = PGESIZE - (srcva - va0);
    	if(n > max) {
      		n = max;
		}

    	char *p = (char *) (pa0 + (srcva - va0));
    	
		while (n > 0) {	
      		if (*p == '\0') {
        		*dst = '\0';
        		got_null = 1;
        		break;
      		} 
			else {
        		*dst = *p;
      		}
      		--n;
      		--max;
      		p++;
      		dst++;
    	}

    	srcva = va0 + PGESIZE;
  	}
  	if (got_null) {
    	return 0;
  	} 
	else {
    	return -1;
  	}
}

