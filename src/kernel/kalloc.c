// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "paging.h"
#include "locks.h"
#include "../libc/include/stdio.h"
#include "../libc/include/string.h"
#include <stdint.h>

void freerange(void *pa_start, void *pa_end);
#define PGROUNDUP(sz)  (((sz)+PGESIZE-1) & ~(PGESIZE-1))

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

void
pmm_init(void)
{
  initlock(&kmem.lock, "kmem");
  freerange(mem_end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  int i = 0;
  char *p;
  p = (char*)PGROUNDUP((uint32_t)pa_start);
  for(; p + PGESIZE <= (char *)pa_end; p += PGESIZE) {
    kfree(p);
    i++;
  }
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;

  if(((uint32_t)pa % PGESIZE) != 0 || (char *)pa < mem_end || (uint32_t)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGESIZE);

  r = (struct run*)pa;

  acquire_lock(&kmem.lock);
  r->next = kmem.freelist;
  kmem.freelist = r;
  release_lock(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire_lock(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  release_lock(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGESIZE); // fill with junk
  return (void*)r;
}
