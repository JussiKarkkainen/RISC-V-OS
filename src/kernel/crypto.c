#include <stddef.h>
#include <stdint.h>


typedef struct {
    uint64_t key[2];
} siphash_key_t;

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


uint64_t rng() {
    randinit(1);
    return rand(); 
}



uint32_t siphash_4u32(uint32_t, uint32_t, uint32_t, uint32_t, siphash_key_t key) {

}
