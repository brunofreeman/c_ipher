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
    char* ciphertext = malloc((len + 1) * sizeof(char));

    for (size_t i = 0; i < len; i++) {
        bool upper = 'A' <= plaintext[i] && plaintext[i] <= 'Z';
        bool lower = 'a' <= plaintext[i] && plaintext[i] <= 'z';
        ciphertext[i] = (upper) * ('A' + 'Z' - plaintext[i]) +
                        (lower) * ('a' + 'z' - plaintext[i]) +
                        (!upper && !lower) * plaintext[i];
    }

    ciphertext[len] = '\0';
    return ciphertext;
}

char* atbash_decrypt(const char* ciphertext) {
    return atbash_encrypt(ciphertext);
}

char* caesar_encrypt(const char* plaintext, const int shift) {
    size_t len = strlen(plaintext);
    char* ciphertext = malloc((len + 1) * sizeof(char));

    for (size_t i = 0; i < len; i++) {
        bool upper = 'A' <= plaintext[i] && plaintext[i] <= 'Z';
        bool lower = 'a' <= plaintext[i] && plaintext[i] <= 'z';
        ciphertext[i] = (upper) * (alpha_mod(plaintext[i] - 'A' + shift) + 'A') +
                        (lower) * (alpha_mod(plaintext[i] - 'a' + shift) + 'a') +
                        (!upper && !lower) * plaintext[i];
    }

    ciphertext[len] = '\0';
    return ciphertext;
}

char* caesar_decrypt(const char* ciphertext, const int shift) {
    return caesar_encrypt(ciphertext, -shift);
}

char* vigenere_encrypt(const char* plaintext, const char* key) {
    size_t len = strlen(plaintext);
    char* ciphertext = malloc((len + 1) * sizeof(char));

    size_t key_len = strlen(key);
    size_t key_idx = 0;

    for (size_t i = 0; i < len; i++) {
        bool upper = 'A' <= plaintext[i] && plaintext[i] <= 'Z';
        bool lower = 'a' <= plaintext[i] && plaintext[i] <= 'z';
        size_t shift = key[key_idx] - 'A';
        ciphertext[i] = (upper) * (alpha_mod(plaintext[i] - 'A' + shift) + 'A') +
                        (lower) * (alpha_mod(plaintext[i] - 'a' + shift) + 'a') +
                        (!upper && !lower) * plaintext[i];
        key_idx += upper || lower;
        key_idx %= key_len;
    }

    ciphertext[len] = '\0';
    return ciphertext;
}

char* vigenere_decrypt(const char* ciphertext, const char* key) {
    size_t key_len = strlen(key);
    char* anti_key = malloc((key_len + 1) * sizeof(char));

    for (size_t i = 0; i < key_len; i++) {
        anti_key[i] = (key[i] == 'A') * 'A' +
                      (key[i] != 'A') * ('B' + 'Z' - key[i]);
    }

    char* plaintext = vigenere_encrypt(ciphertext, anti_key);

    free(anti_key);
    return plaintext;
}