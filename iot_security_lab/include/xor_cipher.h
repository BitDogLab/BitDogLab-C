#ifndef XOR_CIPHER_H
#define XOR_CIPHER_H
#include <stdint.h>
#include <stddef.h>
void xor_encrypt(const uint8_t *input, uint8_t *output, size_t len, uint8_t key);
#endif