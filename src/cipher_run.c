#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ciphers.h"

int usage() {
    printf("usage: ./c_ipher --[cipher_name] -[e/d] [\"message\"] OR ./c_ipher --help\n");
    return 1;
}

int help() {
    usage();
    printf("\t1) the relative path to the executable may be different\n");
    printf("\t2) -e --> encrypt message, -d --> decrypt message\n");
    printf("\t3) put your message in quotes; escape quotes that are part of the message\n");
    printf("available ciphers:\n");
    printf("\t--atbash\n");
    return 0;
}

int main(int argc, char** argv) {
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        return help();
    }
    if (argc != 4) {
        return usage();
    }

    char* ciphertext;

    if (strcmp(argv[1], "--atbash") == 0) {
        if (strcmp(argv[2], "-e") != 0 && strcmp(argv[2], "-d") != 0) {
            return usage();
        }
        ciphertext = atbash(argv[3]);
        printf("%s\n", ciphertext);
    } else {
        return usage();
    }

    free(ciphertext);
    return 0;
}