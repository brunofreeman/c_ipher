#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ciphers.h"

#define ALPHABET_LEN 26

/*
 * @param dividend the number to take the
 *        modulus of
 * @param divisor the modulus
 */
static int mod(const int dividend, const int divisor) {
    return ((dividend % divisor) + divisor) % divisor;
}

/*
 * @param dividend the number to take the
 *        modulus of at mod 26, the alphabet
 *        length
 */
static int alpha_mod(const int dividend) {
    return mod(dividend, ALPHABET_LEN);
}

/*
 * @param num the number to find the
 *        modular multiplicative inverse of,
 *        mod 26, the alphabet length
 * @contract num must be coprime with the
 *           alphabet length, 26
 */
static int alpha_mod_inverse(const int num) {
    int inverse = 0;
    while (alpha_mod(++inverse * num) != 1);
    return inverse;
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

    char anti_key[key_len + 1];
    anti_key[key_len] = '\0';

    for (size_t i = 0; i < key_len; i++) {
        anti_key[i] = (key[i] == 'A') * 'A' +
                      (key[i] != 'A') * ('B' + 'Z' - key[i]);
    }

    char* plaintext = vigenere_encrypt(ciphertext, anti_key);

    return plaintext;
}

char* affine_encrypt(const char* plaintext, const int step, const int shift) {
    size_t len = strlen(plaintext);
    char* ciphertext = malloc((len + 1) * sizeof(char));

    for (size_t i = 0; i < len; i++) {
        bool upper = 'A' <= plaintext[i] && plaintext[i] <= 'Z';
        bool lower = 'a' <= plaintext[i] && plaintext[i] <= 'z';
        ciphertext[i] = (upper) * (alpha_mod((plaintext[i] - 'A') * step + shift) + 'A') +
                        (lower) * (alpha_mod((plaintext[i] - 'a') * step + shift) + 'a') +
                        (!upper && !lower) * plaintext[i];
    }

    ciphertext[len] = '\0';
    return ciphertext;
}

char* affine_decrypt(const char* ciphertext, const int step, const int shift) {
    size_t len = strlen(ciphertext);
    char* plaintext = malloc((len + 1) * sizeof(char));

    int mod_inverse = alpha_mod_inverse(step);

    for (size_t i = 0; i < len; i++) {
        bool upper = 'A' <= ciphertext[i] && ciphertext[i] <= 'Z';
        bool lower = 'a' <= ciphertext[i] && ciphertext[i] <= 'z';
        plaintext[i] = (upper) * (alpha_mod(mod_inverse * (ciphertext[i] - 'A' - shift)) + 'A') +
                       (lower) * (alpha_mod(mod_inverse * (ciphertext[i] - 'a' - shift)) + 'a') +
                       (!upper && !lower) * ciphertext[i];
    }

    plaintext[len] = '\0';
    return plaintext;
}