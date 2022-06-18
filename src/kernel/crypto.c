#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint64_t key[2];
} siphash_key_t;

uint32_t randrsl[256], randcnt;

static uint32_t mm[256];
static uint32_t aa = 0, bb = 0, cc = 0;

void isaac() {
    register ub4 i,x,y;

   cc = cc + 1;    
   bb = bb + cc;   

   for (i=0; i<256; ++i) {
        x = mm[i];
        switch (i%4) {
            case 0: aa = aa^(aa<<13); break;
            case 1: aa = aa^(aa>>6); break;
            case 2: aa = aa^(aa<<2); break;
            case 3: aa = aa^(aa>>16); break;
        }
        aa              = mm[(i+128)%256] + aa;
        mm[i]      = y  = mm[(x>>2)%256] + aa + bb;
        randrsl[i] = bb = mm[(y>>10)%256] + x;
    }
}

uint64_t rng() {


uint32_t siphash_4u32(uint32_t, uint32_t, uint32_t, uint32_t, siphash_key_t key) {

}
