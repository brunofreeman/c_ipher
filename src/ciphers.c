#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ciphers.h"

#define ALPHABET_LEN 26
#define BACONIAN_LEN 5

/*
 * @param dividend the number to take the
 *        modulus of
 * @param divisor the modulus
 * @return the mathematical modulus of
 *         dividend by divisor
 */
static int mod(const int dividend, const int divisor) {
    return ((dividend % divisor) + divisor) % divisor;
}

/*
 * @param dividend the number to take the
 *        modulus of at mod 26, the alphabet
 *        length
 * @return mathematical modulus of dividend
 *         by 26
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
 * @return the mod 26 inverse of num
 */
static int alpha_mod_inverse(const int num) {
    int inverse = 0;
    while (alpha_mod(++inverse * num) != 1);
    return inverse;
}

char* affine_encrypt(const char* plaintext, const int step, const int shift) {
    size_t len = strlen(plaintext);
    char* ciphertext = malloc((len + 1) * sizeof(char));

    for (size_t i = 0; i < len; i++) {
        char c = plaintext[i];
        bool upper = 'A' <= c && c <= 'Z';
        bool lower = 'a' <= c && c <= 'z';
        ciphertext[i] = (upper) * (alpha_mod((c - 'A') * step + shift) + 'A') +
                        (lower) * (alpha_mod((c - 'a') * step + shift) + 'a') +
                        (!upper && !lower) * c;
    }

    ciphertext[len] = '\0';
    return ciphertext;
}

char* affine_decrypt(const char* ciphertext, const int step, const int shift) {
    size_t len = strlen(ciphertext);
    char* plaintext = malloc((len + 1) * sizeof(char));

    int mod_inverse = alpha_mod_inverse(step);

    for (size_t i = 0; i < len; i++) {
        char c = ciphertext[i];
        bool upper = 'A' <= c && c <= 'Z';
        bool lower = 'a' <= c && c <= 'z';
        plaintext[i] = (upper) * (alpha_mod(mod_inverse * (c - 'A' - shift)) + 'A') +
                       (lower) * (alpha_mod(mod_inverse * (c - 'a' - shift)) + 'a') +
                       (!upper && !lower) * c;
    }

    plaintext[len] = '\0';
    return plaintext;
}

char* atbash_encrypt(const char* plaintext) {
    size_t len = strlen(plaintext);
    char* ciphertext = malloc((len + 1) * sizeof(char));

    for (size_t i = 0; i < len; i++) {
        char c = plaintext[i];
        bool upper = 'A' <= c && c <= 'Z';
        bool lower = 'a' <= c && c <= 'z';
        ciphertext[i] = (upper) * ('A' + 'Z' - c) +
                        (lower) * ('a' + 'z' - c) +
                        (!upper && !lower) * c;
    }

    ciphertext[len] = '\0';
    return ciphertext;
}

char* atbash_decrypt(const char* ciphertext) {
    return atbash_encrypt(ciphertext);
}

/*
 * @param c the char to encrypt
 * @contract c is a letter
 * @contract caller is responsible for
 *           freeing the return
 * @return the Baconian encryption of c
 */
char* to_baconian(const char c) {
    char* baconian = malloc((1 + BACONIAN_LEN) * sizeof(char));
    baconian[BACONIAN_LEN] = '\0';

    bool upper = 'A' <= c && c <= 'Z';
    bool lower = 'a' <= c && c <= 'z';

    unsigned char code = c - (upper * 'A' + lower * 'a');

    for (size_t i = 0; i < BACONIAN_LEN; i++) {
        bool curr_bit = code % 2 == 1;
        baconian[BACONIAN_LEN - 1 - i] = curr_bit * 'B' + !curr_bit * 'A' + lower * ('a' - 'A');
        code >>= 1;
    }

    return baconian;
}

char* baconian_encrypt(const char* plaintext) {
    size_t ciphertext_len = 0;
    size_t plaintext_idx = 0;

    while (plaintext[plaintext_idx]) {
        char c = plaintext[plaintext_idx];
        bool upper = 'A' <= c && c <= 'Z';
        bool lower = 'a' <= c && c <= 'z';
        ciphertext_len += BACONIAN_LEN * (upper || lower) + (!upper && !lower);
        plaintext_idx++;
    }
    ciphertext_len++;

    char* ciphertext = malloc(ciphertext_len * sizeof(char));
    ciphertext[ciphertext_len - 1] = '\0';

    size_t ciphertext_idx = 0;
    plaintext_idx = 0;

    while (plaintext[plaintext_idx]) {
        char c = plaintext[plaintext_idx];
        bool upper = 'A' <= c && c <= 'Z';
        bool lower = 'a' <= c && c <= 'z';

        if (upper || lower) {
            char* baconian = to_baconian(c);
            strcpy(&ciphertext[ciphertext_idx], baconian);
            free(baconian);
            ciphertext_idx += BACONIAN_LEN;
        } else {
            ciphertext[ciphertext_idx++] = plaintext[plaintext_idx];
        }

        plaintext_idx++;
    }

    return ciphertext;
}

/*
 * @param baconian the string to decrypt
 *        to a single char
 * @contract baconian contains at least
 *           five chars, not including
 *           an optional null terminator
 * @return the Baconian decryption of the
 *         first five chars of baconian or
 *         '\0' on a failure
 */
char from_baconian(const char* baconian) {
    char c = 0;

    bool upper = false;
    bool lower = false;

    for (size_t i = 0; i < BACONIAN_LEN; i++) {
        char curr_bac = baconian[i];

        bool curr_upper = 'A' <= curr_bac && curr_bac <= 'Z';
        bool curr_lower = 'a' <= curr_bac && curr_bac <= 'z';

        if (!curr_upper && !curr_lower) return '\0';

        upper = upper || curr_upper;
        lower = lower || curr_lower;

        if (upper && lower) return '\0';

        c += (curr_upper && curr_bac == 'B') || (curr_lower && curr_bac == 'b');

        c <<= (i != BACONIAN_LEN - 1) * 1;
    }

    c += upper * 'A' + lower * 'a';

    return c;
}

char* baconian_decrypt(const char* ciphertext) {
    char* baconian_buf = calloc(BACONIAN_LEN, sizeof(char));
    size_t ciphertext_idx = 0;
    size_t plaintext_len = 0;

    size_t ciphertext_len = strlen(ciphertext);

    char* plaintext = malloc((ciphertext_len + 1) * sizeof(char));
    plaintext[ciphertext_len] = '\0';

    while (ciphertext[ciphertext_idx]) {
        for (size_t i = 0; i < BACONIAN_LEN - 1; i++) baconian_buf[i] = baconian_buf[i + 1];
        baconian_buf[BACONIAN_LEN - 1] = ciphertext[ciphertext_idx];

        char c = from_baconian(baconian_buf);

        if (c) {
            plaintext[plaintext_len++] = c;
            ciphertext_idx += BACONIAN_LEN;
            if (ciphertext_idx >= ciphertext_len) {
                size_t remainder = BACONIAN_LEN - (ciphertext_idx - ciphertext_len) - 1;
                for (size_t i = 0; i < remainder; i++) {
                    plaintext[plaintext_len++] = ciphertext[ciphertext_idx - BACONIAN_LEN + 1 + i];
                }
                ciphertext_idx = ciphertext_len;
            } else {
                strncpy(baconian_buf, &ciphertext[ciphertext_idx - BACONIAN_LEN], BACONIAN_LEN);
            }
        } else {
            ciphertext_idx++;
            if (ciphertext_idx >= BACONIAN_LEN) {
                plaintext[plaintext_len++] = ciphertext[ciphertext_idx - BACONIAN_LEN];
                if (ciphertext_idx == ciphertext_len) {
                    for (size_t i = 0; i < BACONIAN_LEN - 1; i++) {
                        plaintext[plaintext_len++] = ciphertext[ciphertext_idx - BACONIAN_LEN + 1 + i];
                    }
                }
            }
        }
    }

    free(baconian_buf);

    plaintext[plaintext_len++] = '\0';
    char* new_plaintext = realloc(plaintext, plaintext_len * sizeof(char));
    if (!new_plaintext) {
        return plaintext;
    } else {
        return new_plaintext;
    }
}

char* caesar_encrypt(const char* plaintext, const int shift) {
    size_t len = strlen(plaintext);
    char* ciphertext = malloc((len + 1) * sizeof(char));

    for (size_t i = 0; i < len; i++) {
        char c = plaintext[i];
        bool upper = 'A' <= c && c <= 'Z';
        bool lower = 'a' <= c && c <= 'z';
        ciphertext[i] = (upper) * (alpha_mod(c - 'A' + shift) + 'A') +
                        (lower) * (alpha_mod(c - 'a' + shift) + 'a') +
                        (!upper && !lower) * c;
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
        char c = plaintext[i];
        bool upper = 'A' <= c && c <= 'Z';
        bool lower = 'a' <= c && c <= 'z';
        size_t shift = key[key_idx] - 'A';
        ciphertext[i] = (upper) * (alpha_mod(c - 'A' + shift) + 'A') +
                        (lower) * (alpha_mod(c - 'a' + shift) + 'a') +
                        (!upper && !lower) * c;
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