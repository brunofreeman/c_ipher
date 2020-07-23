#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "ciphers.h"

#define NUM_TESTS 8
#define TEST_NAME_LEN 17

#define DIR_EXT_NULL_LEN (7 + 4 + 1)
#define TEST_DIR "./test/"
#define TEST_EXT ".txt"

#define ARGS_TOKEN "???"

enum read_mode_e {
    ENTRY_INFO = 0,
    INPUT      = 1,
    OUTPUT     = 2,
    FAILURE    = 3
};

enum cipher_e {
    AFFINE   = 0,
    ATBASH   = 1,
    CAESAR   = 2,
    VIGENERE = 3
};

enum cipher_direction_e {
    ENCRYPTION = 0,
    DECRYPTION = 1
};

/*
 * @member info needs to be freed
 */
typedef struct {
    char** info;
    bool success;
} case_info_t;

/*
 * @member text needs to be freed
 */
typedef struct {
    char* entry;
    bool success;
} entry_t;

typedef struct {
    enum cipher_e cipher;
    enum cipher_direction_e cipher_direction;
    size_t max_text_len;
    size_t num_args;
    size_t max_arg_len;
    bool success;
} test_info_t;

typedef struct {
    bool passed;
} test_result_t;

test_info_t read_test_info(char* line_buf, size_t* line_buf_size, FILE* file, ssize_t* line_size);

case_info_t parse_entry_info(char* line_buf, size_t num_args);

entry_t read_input_output(char* line_buf, size_t* line_buf_size, FILE* file, ssize_t* line_size, bool input);

test_result_t conduct_test(char* input, char* output, enum cipher_e cipher, enum cipher_direction_e cipher_direction);

void print_test_result(size_t count, size_t passed, enum cipher_e cipher, enum cipher_direction_e cipher_direction);

int main(int argc, char** argv) {

    printf("Testing...\n");

    char test_names[NUM_TESTS][TEST_NAME_LEN] = {
              "affine_encrypt",   "affine_decrypt",
              "atbash_encrypt",   "atbash_decrypt",
              "caesar_encrypt",   "caesar_decrypt",
            "vigenere_encrypt", "vigenere_decrypt"
    };

    for (size_t i = 0; i < NUM_TESTS; i++) {

        char file_name[TEST_NAME_LEN + DIR_EXT_NULL_LEN];
        snprintf(file_name, sizeof(file_name), "%s%s%s", TEST_DIR, test_names[i], TEST_EXT);

        FILE* file = fopen(file_name, "r");
        if (!file) return EXIT_FAILURE;

        char* line_buf = NULL;
        size_t line_buf_size = 0;
        ssize_t line_size;

        test_info_t test_info = read_test_info(line_buf, &line_buf_size, file, &line_size);
        if (!test_info.success) {
            fclose(file);
            free(line_buf);
            return EXIT_FAILURE;
        }

        char*  input = malloc((test_info.max_text_len + 1) * sizeof(char));
        char* output = malloc((test_info.max_text_len + 1) * sizeof(char));

        enum read_mode_e read_mode = ENTRY_INFO;

        case_info_t entry_info;
        entry_t input_read;
        entry_t output_read;

        size_t test_count   = 0;
        size_t tests_passed = 0;

        bool reading = true;

        while (reading) switch (read_mode) {
            case ENTRY_INFO: {
                line_size = getline(&line_buf, &line_buf_size, file);
                if (line_size == EOF) {
                    reading = false;
                } else {
                    entry_info = parse_entry_info(line_buf, test_info.num_args);
                    if (!entry_info.success) {
                        read_mode = FAILURE;
                        reading = false;
                    } else {
                        read_mode = INPUT;
                    }
                }
                break;
            } case INPUT: {
                input_read = read_input_output(line_buf, &line_buf_size, file, &line_size, true);
                if (!input_read.success) {
                    read_mode = FAILURE;
                    reading = false;
                } else {
                    read_mode = OUTPUT;
                }
                break;
            } case OUTPUT: {
                output_read = read_input_output(line_buf, &line_buf_size, file, &line_size, true);
                if (!output_read.success) {
                    read_mode = FAILURE;
                    reading = false;
                } else {
                    test_result_t test_result = conduct_test(input, output, test_info.cipher, test_info.cipher_direction);
                    test_count++;
                    if (test_result.passed) {
                        tests_passed++;
                    } else {
                        // TODO: add more info
                        printf("Case failed!\n");
                    }
                    read_mode = ENTRY_INFO;
                }
                break;
            } default: {
                read_mode = FAILURE;
                reading = false;
            }
        }

        fclose(file);
        free(line_buf);
        free(input);
        free(output);

        if (read_mode == FAILURE) return EXIT_FAILURE;
        else print_test_result(test_count, tests_passed, test_info.cipher, test_info.cipher_direction);
    }

    return EXIT_SUCCESS;
}

test_info_t read_test_info(char* line_buf, size_t* line_buf_size, FILE* file, ssize_t* line_size) {

}

case_info_t parse_entry_info(char* line_buf, size_t num_args) {

}

entry_t read_input_output(char* line_buf, size_t* line_buf_size, FILE* file, ssize_t* line_size, bool input) {

}

test_result_t conduct_test(char* input, char* output, enum cipher_e cipher, enum cipher_direction_e cipher_direction) {

}

void print_test_result(size_t count, size_t passed, enum cipher_e cipher, enum cipher_direction_e cipher_direction) {

}