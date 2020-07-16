#include <stdlib.h>
#include <string.h>
#include "ciphers.h"

char* atbash(const char* plaintext) {
    size_t len = strlen(plaintext);
    char* ciphertext = malloc(len * sizeof(char));
    for (size_t i = 0; i < len; i++) {
        if ('A' <= plaintext[i] && plaintext[i] <= 'Z') {
            ciphertext[i] = 'A' + 'Z' - plaintext[i];
        } else if ('a' <= plaintext[i] && plaintext[i] <= 'z') {
            ciphertext[i] = 'a' + 'z' - plaintext[i];
        } else {
            ciphertext[i] = plaintext[i];
        }
    }
    return ciphertext;
}
