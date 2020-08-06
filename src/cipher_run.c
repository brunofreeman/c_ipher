#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ciphers.h"
#include "validate.h"

void print_cipher_help(const char* name, const char* extra_args) {
    printf("\t--%s\n\t\t* %s\n", name, extra_args);
}

int usage() {
    printf("usage: ./c_ipher --[cipher_name] -[e/d] (extra arguments) ['message'] OR ./c_ipher --help\n");
    return EXIT_FAILURE;
}

int help() {
    usage();
    printf("\t1) the relative path to the executable may be different\n");
    printf("\t2) -e --> encrypt, -d --> decrypt\n");
    printf("\t3) put your message in single quotes; some characters may need to be escaped\n");
    printf("\t4) keys should consist of only uppercase letters\n");
    printf("available ciphers and their required extra arguments:\n");
    print_cipher_help("affine", "step shift");
    print_cipher_help("atbash", "N/A");
    print_cipher_help("baconian", "N/A");
    print_cipher_help("caesar", "shift");
    print_cipher_help("vigenere", "key");
    return EXIT_SUCCESS;
}

int main(int argc, char** argv) {
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        return help();
    } else if (argc < 2) {
        return usage();
    }

    char* text;

    if (strcmp(argv[1], "--affine") == 0) {
        if (argc != 6) return usage();

        int step = (int) strtol(argv[3], NULL, 10);
        if ((argv[3][0] != '0' && step == 0) || !are_coprime(step, 26)) return usage();

        int shift = (int) strtol(argv[4], NULL, 10);
        if (argv[4][0] != '0' && shift == 0) return usage();

        if (strcmp(argv[2], "-e") == 0) {
            text = affine_encrypt(argv[5], step, shift);
        } else if (strcmp(argv[2], "-d") == 0) {
            text = affine_decrypt(argv[5], step, shift);
        } else return usage();

    } else if (strcmp(argv[1], "--atbash") == 0) {
        if (argc != 4) return usage();

        if (strcmp(argv[2], "-e") == 0) {
            text = atbash_encrypt(argv[3]);
        } else if (strcmp(argv[2], "-d") == 0) {
            text = atbash_decrypt(argv[3]);
        } else return usage();

    } else if (strcmp(argv[1], "--baconian") == 0) {
        if (argc != 4) return usage();

        if (strcmp(argv[2], "-e") == 0) {
            text = baconian_encrypt(argv[3]);
        } else if (strcmp(argv[2], "-d") == 0) {
            text = baconian_decrypt(argv[3]);
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

    } else if (strcmp(argv[1], "--vigenere") == 0) {
        if (argc != 5) return usage();

        if (!is_only_upper_alpha(argv[3])) return usage();

        if (strcmp(argv[2], "-e") == 0) {
            text = vigenere_encrypt(argv[4], argv[3]);
        } else if (strcmp(argv[2], "-d") == 0) {
            text = vigenere_decrypt(argv[4], argv[3]);
        } else return usage();

    } else return usage();

    printf("%s\n", text);

    free(text);
    return EXIT_SUCCESS;
}