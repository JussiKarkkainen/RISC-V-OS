#ifndef CRYPTO_H
#define CRYPTO_H

uint8_t *key_gen(void);
int siphash24(uint8_t *in, int inlen, uint8_t key[16], uint8_t out[8]);

#endif
