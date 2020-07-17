#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ciphers.h"

#define START_TEST_ID "#START_TEST\n"
#define END_TEST_ID "#END_TEST"
#define MAX_LEN_ID "#MAX_LEN\n"
#define PT_ID "#PT\n"
#define CT_ID "#CT\n"

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

    line_size = getline(&line_buf, &line_buf_size, file);
    if (line_size == EOF || strcmp(line_buf, START_TEST_ID) != 0) {
        fclose(file);
        return EXIT_FAILURE;
    }

    size_t max_len = get_max_len(line_buf, &line_buf_size, file, &line_size);

    if (max_len == 0) {
        fclose(file);
        free(line_buf);
        return EXIT_FAILURE;
    }

    char* plaintext = malloc(max_len * sizeof(char));
    char* ciphertext = malloc(max_len * sizeof(char));

    int read_mode = 1;

    while (line_size != EOF) {
        line_size = getline(&line_buf, &line_buf_size, file);
        if (line_size > max_len) {
            read_mode = -1;
            break;
        }
        if (strcmp(line_buf, END_TEST_ID) == 0) {
            read_mode = 0;
            break;
        }
        switch (read_mode++) {
            case 2:
                strncpy(plaintext, line_buf, line_size - 1);
                break;
            case 4:
                strncpy(ciphertext, line_buf, line_size - 1);
                printf("%20s: %s\n", "Plaintext", plaintext);
                printf("%20s: %s\n", "Correct ciphertext", ciphertext);
                char* generated_ct = atbash(plaintext);
                printf("%20s: %s\n", "Generated ciphertext", generated_ct);
                free(generated_ct);
                memset(&plaintext[0], '\0', sizeof(plaintext));
                memset(&ciphertext[0], '\0', sizeof(ciphertext));
                read_mode = 1;
                break;
        }
    }

    fclose(file);

    free(line_buf);
    line_buf = NULL;

    free(plaintext);
    free(ciphertext);

    return read_mode == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}