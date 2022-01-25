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
int num_pages = heap_size / page_size; 

void init() {

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

void clear(addr) {
    *addr = page_bits->EMPTY;
}

bool is_taken(flags) {
    if (flags & (page_bits->LAST) != 0) {

}

void set_flags() {
}

void kalloc(size_t num_pages) {
    int *start_addr = HEAP_START;
    for (int i=0; 1<=num_pages; i++) {
        int found = 0;
        
        if (is_free(*ptr) == 1) {
            found = 1;


}


void free() {
}


void clear() {
}
