#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ciphers.h"

int usage() {
    printf("usage: ./c_ipher --[cipher_name] -[e/d] ('key/shift/etc') ['message'] OR ./c_ipher --help\n");
    return EXIT_FAILURE;
}

int help() {
    usage();
    printf("\t1) the relative path to the executable may be different\n");
    printf("\t2) -e --> encrypt message, -d --> decrypt message\n");
    printf("\t3) put your message in single quotes; some characters may need to be escaped\n");
    printf("available ciphers:\n");
    printf("\t--atbash\n");
    printf("\t--caesar\n");
    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        return help();
    }

    char* text;

    if (strcmp(argv[1], "--atbash") == 0) {
        if (argc != 4) return usage();

        if (strcmp(argv[2], "-e") == 0) {
            text = atbash_encrypt(argv[3]);
        } else if (strcmp(argv[2], "-d") == 0) {
            text = atbash_decrypt(argv[3]);
        } else return usage();

    } else if (strcmp(argv[1], "--caesar") == 0) {
        if (argc != 5) return usage();

        int shift = (int) strtol(argv[3], NULL, 10);
        if (argv[3][0] != '0' && shift == 0) return usage();

        if (strcmp(argv[2], "-e") == 0) {
            text = caesar_encrypt(argv[4], shift);
        } else if (strcmp(argv[2], "-d") == 0) {
            text = caesar_decrypt(argv[4], shift);
        } else return usage();

    } else return usage();

    printf("%s\n", text);

    free(text);
    return EXIT_SUCCESS;
}