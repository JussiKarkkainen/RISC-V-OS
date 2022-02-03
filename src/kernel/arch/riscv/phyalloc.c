#include <stdint.h>

/*
This implementation uses 2 bits to describe pages, EMPTY, TAKEN, LAST.
The LAST bit could propably be removed to simplify the allocator

Remember you have functions like memset() implemented.
*/

typedef enum {
    EMPTY = 0;
    TAKEN = 1;
} page_bits;

// Aligns memory to a 4096 page
int align(int value, int align) {
    int c = (1 << align) - 1;
    return value + (~value & c);   
}

int align = 12;
int ALLOC_START;
int page_size = 1 << 12;
int num_pages = HEAP_SIZE / page_size; 

void clear(*addr) {
    *addr = page_bits->EMPTY;
}

void init() {

    // Clear pages
    int *start_addr = HEAP_START;
    for (int i=0; i<= num_pages; i++) {
        clear(start_addr + i);
    }

    // Determine location of useful memory
    ALLOC_START = align(HEAP_START + (num_pages * page_size,
                        align);
}

// Return true if page is free
bool is_free(uint32_t *flags) {
    if (*flags & (page_bits->TAKEN) == 0) {
        return true;
    }
    else {
        return false;
    }
}

// Sets bits in descriptor
void set_flags(int *page_descriptor, int bits) {
    switch (bits) {
        case -1: *page_descriptor = page_bits->EMPTY;
            break;

        case 0: *page_descriptor = page_bits->TAKEN;
            break;
    }
}

uint32_t *kalloc(size_t num_pages) {
    int *start_addr = HEAP_START;

    // Iterate through all page descriptors to see if they are free
    for (int i=0; 1<=(num_pages-pages); i++) {
        int found = 0;
        
        if (is_free(*start_addr) == 1) {
            found = 1;
            // Check if there are enough contiguos pages to be allocated, if not, look elsewhere
            for (int j=0; j<=num_pages; j++) {
                if (is_free(start_addr + j) == 0) {
                    found = 0;
                    break;
                }
            }
        }

        if (found == 1) {
            for (int k=0; k<=pages-1; i++) {
                set_flags((*ptr + k));

                return (ALLOC_START + page_size * i);
            }
        }

    return NULL;
    
    }
}

// This can be simplified

// Set descriptors to zero
void free(uint32_t *addr) {
    if (addr != NULL) {

        start = HEAP_START + (addr - ALLOC_START) / PAGE_SIZE;
        page_info *p = start;

        while (is_taken(*p) == true && is_last(*p) != true) {
            clear(*p);
            p = p + 1;
        }
        clear(*p);
    }

}


// What and when to return

// Allocate and clear memory to zero
void *clear(int pages) {
    uint32_t *ptr = alloc(pages);
    if (ptr != NULL) {
        alloc_size = (PAGE_SIZE * pages) / 8;
        for (int i=0; i<=size; i++) {
            ptr = ptr + i;
            *ptr = 0;
        

        }
    }

}


void test_allocator() {
    













