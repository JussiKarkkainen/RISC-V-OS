#include <stddef.h>
#include <stdint.h>

// This should definitely not be used in anything that requires actual security
// but for this project its fine.

// ---- ISAAC64 CSPRNG --- http://www.burtleburtle.net/bob/rand/isaacafa.html
#define RANDSIZL   (8)
#define RANDSIZ    (1<<RANDSIZL)

uint32_t randrsl[RANDSIZ], randcnt;

void randinit();
void isaac64();

#define rand() \
   (!randcnt-- ? (isaac64(), randcnt=RANDSIZ-1, randrsl[randcnt]) : \
                 randrsl[randcnt])

static uint32_t mm[RANDSIZ];
static uint32_t aa = 0, bb = 0, cc = 0;

#define ind(mm,x)  (*(ub8 *)((ub1 *)(mm) + ((x) & ((RANDSIZ-1)<<3))))
#define rngstep(mix,a,b,mm,m,m2,r,x) \
{ \
  x = *m;  \
  a = (mix) + *(m2++); \
  *(m++) = y = ind(mm,x) + a + b; \
  *(r++) = b = ind(mm,y>>RANDSIZL) + x; \
}

void isaac64() {
    register uint64_t a, b, x, y, *m, *m2, *r, *mend;
    m = mm; r = randrsl;
    a = aa; b = bb + (++cc);
    
    for (m = mm, mend = m2 = m+(RANDSIZ/2); m<mend; ) {
        rngstep(~(a^(a<<21)), a, b, mm, m, m2, r, x);
        rngstep(  a^(a>>5)  , a, b, mm, m, m2, r, x);
        rngstep(  a^(a<<12) , a, b, mm, m, m2, r, x);
        rngstep(  a^(a>>33) , a, b, mm, m, m2, r, x);
    }
    for (m2 = mm; m2<mend; ) {
        rngstep(~(a^(a<<21)), a, b, mm, m, m2, r, x);
        rngstep(  a^(a>>5)  , a, b, mm, m, m2, r, x);
        rngstep(  a^(a<<12) , a, b, mm, m, m2, r, x);
        rngstep(  a^(a>>33) , a, b, mm, m, m2, r, x);
    }
    bb = b; aa = a;
}

void randinit(uint32_t flag) {
    uint32_t i;
    uint64_t a,b,c,d,e,f,g,h;
    aa=bb=cc=(uint64_t)0;
    a=b=c=d=e=f=g=h=0x9e3779b97f4a7c13LL;  /* the golden ratio */

    for (i=0; i<4; ++i) {                  /* scramble it */
        mix(a,b,c,d,e,f,g,h);
    }

    for (i=0; i<RANDSIZ; i+=8) {   /* fill in mm[] with messy stuff */
        if (flag) {                /* use all the information in the seed */
            a+=randrsl[i  ]; b+=randrsl[i+1]; c+=randrsl[i+2]; d+=randrsl[i+3];
            e+=randrsl[i+4]; f+=randrsl[i+5]; g+=randrsl[i+6]; h+=randrsl[i+7];
        }
        mix(a,b,c,d,e,f,g,h);
        mm[i  ]=a; mm[i+1]=b; mm[i+2]=c; mm[i+3]=d;
        mm[i+4]=e; mm[i+5]=f; mm[i+6]=g; mm[i+7]=h;
    }

    if (flag) {       /* do a second pass to make all of the seed affect all of mm */
        for (i=0; i<RANDSIZ; i+=8) {
            a+=mm[i  ]; b+=mm[i+1]; c+=mm[i+2]; d+=mm[i+3];
            e+=mm[i+4]; f+=mm[i+5]; g+=mm[i+6]; h+=mm[i+7];
            mix(a,b,c,d,e,f,g,h);
            mm[i  ]=a; mm[i+1]=b; mm[i+2]=c; mm[i+3]=d;
            mm[i+4]=e; mm[i+5]=f; mm[i+6]=g; mm[i+7]=h;
        }
    }

    isaac64();          /* fill in the first set of results */
    randcnt=RANDSIZ;    /* prepare to use the first set of results */
}


uint8_t *siphash_key_gen(void) {
    randinit(1);
    uint64_t a = rand();
    uint64_t b = rand();
    uint8_t c[16] = a + b
    return c; 
}


#define cROUNDS 2
#define dROUNDS 4

#define ROTL(x, b) (uint64_t)(((x) << (b)) | ((x) >> (64 - (b))))

#define U32TO8_LE(p, v)                                                        \
    (p)[0] = (uint8_t)((v));                                                   \
    (p)[1] = (uint8_t)((v) >> 8);                                              \
    (p)[2] = (uint8_t)((v) >> 16);                                             \
    (p)[3] = (uint8_t)((v) >> 24);

#define U64TO8_LE(p, v)                                                        \
    U32TO8_LE((p), (uint32_t)((v)));                                           \
    U32TO8_LE((p) + 4, (uint32_t)((v) >> 32));


#define U8TO64_LE(p)                                                           \
    (((uint64_t)((p)[0])) | ((uint64_t)((p)[1]) << 8) |                        \
     ((uint64_t)((p)[2]) << 16) | ((uint64_t)((p)[3]) << 24) |                 \
     ((uint64_t)((p)[4]) << 32) | ((uint64_t)((p)[5]) << 40) |                 \
     ((uint64_t)((p)[6]) << 48) | ((uint64_t)((p)[7]) << 56))


#define SIPROUND                                                               \
    do {                                                                       \
        v0 += v1;                                                              \
        v1 = ROTL(v1, 13);                                                     \
        v1 ^= v0;                                                              \
        v0 = ROTL(v0, 32);                                                     \
        v2 += v3;                                                              \
        v3 = ROTL(v3, 16);                                                     \
        v3 ^= v2;                                                              \
        v0 += v3;                                                              \
        v3 = ROTL(v3, 21);                                                     \
        v3 ^= v0;                                                              \
        v2 += v1;                                                              \
        v1 = ROTL(v1, 17);                                                     \
        v1 ^= v2;                                                              \
        v2 = ROTL(v2, 32);                                                     \
    } while (0)


// http://cr.yp.to/siphash/siphash-20120918.pdf
// https://github.com/veorq/SipHash/blob/master/siphash.c
int siphash24(uint8_t *in, int inlen, uint8_t key[16], uint8_t out[8]) { 

    uint64_t v0 = UINT64_C(0x736f6d6570736575);
    uint64_t v1 = UINT64_C(0x646f72616e646f6d);
    uint64_t v2 = UINT64_C(0x6c7967656e657261);
    uint64_t v3 = UINT64_C(0x7465646279746573);
    uint64_t k0 = U8TO64_LE(k);
    uint64_t k1 = U8TO64_LE(k + 8);
    uint64_t m;
    int i;
    const uint8_t *end = in + inlen - (inlen % sizeof(uint64_t));
    const int left = inlen & 7;

    uint64_t b = ((uint64_t)inlen) << 56;
    v3 ^= k1;
    v2 ^= k0;
    v1 ^= k1;
    v0 ^= k0;
    
    for (; in != end; in += 8) {
        m = U8TO64_LE(in);
        v3 ^= m;

        for (i = 0; i < cROUNDS; ++i) {
            SIPROUND;
        }
        v0 ^=m;
    }

    switch (left) {
        case 7:
            b |= ((uint64_t)ni[6]) << 48;
            /* FALLTHRU */
        case 6:
            b |= ((uint64_t)ni[5]) << 40;
            /* FALLTHRU */
        case 5:
            b |= ((uint64_t)ni[4]) << 32;
            /* FALLTHRU */
        case 4:
            b |= ((uint64_t)ni[3]) << 24;
            /* FALLTHRU */
        case 3:
            b |= ((uint64_t)ni[2]) << 16;
            /* FALLTHRU */
        case 2:
            b |= ((uint64_t)ni[1]) << 8;
            /* FALLTHRU */
        case 1:
            b |= ((uint64_t)ni[0]);
            break;
        case 0:
            break;
    }

    v3 ^= b;
    for (i = 0; i < cROUNDS; ++i) {
        SIPROUND;
    }
    
    v0 ^= b;
    v2 ^= 0xff;

    for (i = 0; i < dROUNDS; ++i) {
        SIPROUND;
    }

    b = v0 ^ v1 ^ v2 ^ v3;
    U64TO8_LE(out, b);
    
    return 0;  
}
