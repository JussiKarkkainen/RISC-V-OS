#include <stdint.h>


typedef struct {
    uint8_t flags;
} page;


typedef enum {
    EMPTY = 0;
    TAKEN = 1 << 0;
    LAST = 1 << 1;
} page_bits;


void init() {
}


void kalloc(size_t num_pages) {
}


void free() {
}


void clear() {
}
