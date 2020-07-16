#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ciphers.h"

int main(int argc, char** argv) {
    printf("Testing...\n");

    FILE* file = fopen("./def/atbash", "r");
    if (!file) return EXIT_FAILURE;

    char c;
    while ((c = fgetc(file)) != EOF) {
        printf("%c", c);
    }

    /*char* line_buf = NULL;
    size_t line_buf_size = 0;
    ssize_t line_size;

    line_size = getline(&line_buf, &line_buf_size, file);

    char plaintext[500];
    char ciphertext[500];

    while (line_size >= 0) {
        line_size = getline(&line_buf, &line_buf_size, file);
        if (strcmp(line_buf, "CP") == 0) {

        }
    }

    free(line_buf);
    line_buf = NULL;*/

    fclose(file);
    return EXIT_SUCCESS;
}