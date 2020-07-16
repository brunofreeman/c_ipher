#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ciphers.h"

char* atbash(const char* plaintext) {
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