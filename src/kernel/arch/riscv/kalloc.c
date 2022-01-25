#include <stdint.h>

// Need to have one page_info for each page
typedef struct {
    uint8_t flags;
} page_info;


typedef enum {
    EMPTY = 0;
    TAKEN = 1 << 0;
    LAST = 1 << 1;
} page_bits;


int align(int value, int align) {
    int c = (1 << align) - 1;
    return value + (~value & c);   
}

int align = 12;
int ALLOC_START;
int page_size = 1 << 12;

// Initialize descriptors
void init() {
    int num_pages = heap_size / page_size; 
    
    // Clear pages
    int *start_addr = HEAP_START;
    for (int i=0; i<= num_pages; i++) {
        *start_addr = 0;
        start_addr++;
    }

    // Determine location of useful memory
    ALLOC_START = align(HEAP_START + (num_pages * page_size,
                        align);

}


void kalloc(size_t num_pages) {
}


void free() {
}


void clear() {
}
