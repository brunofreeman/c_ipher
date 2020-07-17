#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ciphers.h"

#define MAX_LEN_ID "#MAX_LEN\n"
#define PT_ID "#PT\n"
#define CT_ID "#CT\n"

/*ssize_t read_line(char* line_buf, size_t* line_buf_size, FILE* file) {
    ssize_t line_size;
    line_size = getline(&line_buf, line_buf_size, file);
    if (line_size > 0 && line_buf[line_size - 1] == '\n') {
        line_buf[line_size - 1] = '\0';
        return line_size - 1;
    } else {
        return EOF;
    }
}*/

/*
 * Reads the test's max string length.
 * @return 0 on a failure, the designated max_len otherwise
 */
ssize_t get_max_len(char* line_buf, size_t* line_buf_size, FILE* file, ssize_t* line_size) {
    // check first line for #MAX_LEN
    *line_size = getline(&line_buf, line_buf_size, file);

    if (*line_size == EOF || strcmp(line_buf, MAX_LEN_ID) != 0) return 0;

    // read in max_len, will be 0 on failure
    *line_size = getline(&line_buf, line_buf_size, file);
    return strtoul(line_buf, NULL, 10);
}

int main(int argc, char** argv) {
    printf("Testing...\n");

    FILE* file = fopen("./def/atbash", "r");
    if (!file) return EXIT_FAILURE;

    char* line_buf = NULL;
    size_t line_buf_size = 0;
    ssize_t line_size;

    size_t max_len = get_max_len(line_buf, &line_buf_size, file, &line_size);

    if (max_len == 0) return EXIT_FAILURE;

    char plaintext[max_len];
    char ciphertext[max_len];

    int read_mode = 0;

    while (line_size != EOF) {
        line_size = getline(&line_buf, &line_buf_size, file);
        switch (read_mode++) {
            case 1:
                strncpy(plaintext, line_buf, line_size - 1);
                break;
            case 3:
                strncpy(ciphertext, line_buf, line_size - 1);
                printf("%20s: %s\n", "Plaintext", plaintext);
                printf("%20s: %s\n", "Correct ciphertext", ciphertext);
                char* generated_ct = atbash(plaintext);
                printf("%20s: %s\n", "Generated ciphertext", generated_ct);
                free(generated_ct);
                break;
        }
    }

    free(line_buf);
    line_buf = NULL;

    fclose(file);
    return EXIT_SUCCESS;
}