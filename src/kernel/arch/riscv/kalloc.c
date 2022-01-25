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

bool is_free(*flags) {
    if (*flags & (page_bits->TAKEN) != 0) {
        return 1;
    }
    else {
        return 0;
    }
}

void set_flags(*page_descriptor) {
    *page_descriptor = page_bits->TAKEN
}

void kalloc(size_t num_pages) {
    int *start_addr = HEAP_START;
    for (int i=0; 1<=num_pages; i++) {
        int found = 0;
        
        if (is_free(*start_addr) == 1) {
            found = 1;
            
            for (int j=0; j<=num_pages; j++) {
                if (is_free(start_addr + j) == 0) {
                    found = 0;
                    break;
                }
            }

        }
    }
}


void free() {
}


void clear() {
}
