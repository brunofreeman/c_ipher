#include <stdlib.h>
#include <stdio.h>
#include "ciphers.h"

int main(int argc, char** argv) {
    if (argc != 2) return 1;
    char* atbash_test = atbash(argv[1]);
    printf("%s\n", atbash_test);
    free(atbash_test);
}