#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ciphers.h"

#define ALPHABET_LEN 26

int mod(int dividend, int divisor) {
    return ((dividend % divisor) + divisor) % divisor;
}

int alpha_mod(int dividend) {
    return mod(dividend, ALPHABET_LEN);
}

char* atbash_encrypt(const char* plaintext) {
    size_t len = strlen(plaintext);
    char* ciphertext = malloc(len * sizeof(char));
    for (size_t i = 0; i < len; i++) {
        bool upper = 'A' <= plaintext[i] && plaintext[i] <= 'Z';
        bool lower = 'a' <= plaintext[i] && plaintext[i] <= 'z';
        ciphertext[i] = (upper) * ('A' + 'Z' - plaintext[i]) +
                        (lower) * ('a' + 'z' - plaintext[i]) +
                        (!upper && !lower) * plaintext[i];
    }
    return ciphertext;
}

char* atbash_decrypt(const char* ciphertext) {
    return atbash_encrypt(ciphertext);
}

char* caesar_encrypt(const char* plaintext, const int shift) {
    size_t len = strlen(plaintext);
    char* ciphertext = malloc(len * sizeof(char));
    for (size_t i = 0; i < len; i++) {
        bool upper = 'A' <= plaintext[i] && plaintext[i] <= 'Z';
        bool lower = 'a' <= plaintext[i] && plaintext[i] <= 'z';
        ciphertext[i] = (upper) * (alpha_mod(plaintext[i] - 'A' + shift) + 'A') +
                        (lower) * (alpha_mod(plaintext[i] - 'a' + shift) + 'a') +
                        (!upper && !lower) * plaintext[i];
    }
    return ciphertext;
}

char* caesar_decrypt(const char* ciphertext, const int shift) {
    return caesar_encrypt(ciphertext, -shift);
}