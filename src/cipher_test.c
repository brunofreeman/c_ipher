#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "ciphers.h"

#define NUM_TESTS 8
#define TEST_NAME_LEN 17
#define NUM_CIPHERS 4
#define CIPHER_NAME_LEN 9

#define CIPHER_NAMES_LIST "affine", "atbash", "caesar", "vigenere"
#define TEST_NAMES_LIST \
  "affine_encrypt",   "affine_decrypt", \
  "atbash_encrypt",   "atbash_decrypt", \
  "caesar_encrypt",   "caesar_decrypt", \
"vigenere_encrypt", "vigenere_decrypt"  \

#define NUM_DIRECTIONS 2
#define DIRECTIONS_NAME_LIST "encrypt", "decrypt"
#define DIRECTION_NAME_LEN 8

#define TEST_DIR "./test/"
#define DIR_LEN 7
#define TEST_EXT ".txt"
#define EXT_LEN 4

#define CIPHER_TOKEN "#cipher="
#define DIRECTION_TOKEN "#direction="
#define MAX_TEXT_LEN_TOKEN "#max_text_len="
#define NUM_ARGS_TOKEN "#num_args="
#define MAX_ARG_LEN_TOKEN "#max_arg_len="
#define CIPHER_TOKEN_LEN 8
#define DIRECTION_TOKEN_LEN 11
#define MAX_TEXT_LEN_TOKEN_LEN 14
#define NUM_ARGS_TOKEN_LEN 10
#define MAX_ARG_LEN_TOKEN_LEN 13
#define MAX_TOKEN_LEN 14

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

enum direction_e {
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
    enum direction_e direction;
    size_t max_text_len;
    size_t num_args;
    size_t max_arg_len;
    bool success;
} test_info_t;

typedef struct {
    bool passed;
} test_result_t;

bool starts_with(const char* text, const char* start);

test_info_t read_test_info(char* line_buf, size_t* line_buf_size, FILE* file, ssize_t* line_size);

case_info_t parse_entry_info(char* line_buf, size_t num_args);

entry_t read_input_output(char* line_buf, size_t* line_buf_size, FILE* file, ssize_t* line_size, bool input);

test_result_t conduct_test(char* input, char* output, enum cipher_e cipher, enum direction_e direction);

void print_test_result(size_t count, size_t passed, enum cipher_e cipher, enum direction_e direction);

int main(int argc, char** argv) {
    printf("Running tests...\n");

    char test_names[NUM_TESTS][TEST_NAME_LEN] = {
            TEST_NAMES_LIST
    };

    for (size_t i = 0; i < NUM_TESTS; i++) {

        char file_name[DIR_LEN + TEST_NAME_LEN + EXT_LEN + 1];
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
                    test_result_t test_result = conduct_test(input, output, test_info.cipher, test_info.direction);
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
        else print_test_result(test_count, tests_passed, test_info.cipher, test_info.direction);
    }

    return EXIT_SUCCESS;
}

/*typedef struct {
    enum cipher_e cipher;
    enum direction_e cipher_direction;
    size_t max_text_len;
    size_t num_args;
    size_t max_arg_len;
    bool success;
} test_info_t;*/

bool starts_with(const char* text, const char* start) {
    size_t idx = 0;
    while (text[idx] && start[idx]) {
        if (text[idx] != start[idx]) return false;
        idx++;
    }
    return !start[idx];
}

test_info_t read_test_info(char* line_buf, size_t* line_buf_size, FILE* file, ssize_t* line_size) {
    test_info_t test_info;

    *line_size = getline(&line_buf, line_buf_size, file);

    char cipher_names[NUM_CIPHERS][CIPHER_NAME_LEN] = {
            CIPHER_NAMES_LIST
    };

    int cipher_code = -1;

    for (size_t i = 0; i < NUM_CIPHERS; i++) {
        char potential_token[CIPHER_TOKEN_LEN + CIPHER_NAME_LEN + 1 + 1];
        snprintf(potential_token, sizeof(potential_token), "%s%s\n", CIPHER_TOKEN, cipher_names[i]);
        if (strcmp(line_buf, potential_token) == 0) {
            cipher_code = i;
            break;
        }
    }
    if (cipher_code == -1) {
        test_info.success = false;
        return test_info;
    } else {
        test_info.cipher = (enum cipher_e) cipher_code;
    }

    *line_size = getline(&line_buf, line_buf_size, file);

    char direction_names[NUM_DIRECTIONS][DIRECTION_NAME_LEN] = {
            DIRECTIONS_NAME_LIST
    };

    int direction_code = -1;

    for (size_t i = 0; i < NUM_DIRECTIONS; i++) {
        char potential_token[DIRECTION_TOKEN_LEN + DIRECTION_NAME_LEN + 1 + 1];
        snprintf(potential_token, sizeof(potential_token), "%s%s\n", DIRECTION_TOKEN, direction_names[i]);
        if (strcmp(line_buf, potential_token) == 0) {
            direction_code = i;
            break;
        }
    }
    if (direction_code == -1) {
        test_info.success = false;
        return test_info;
    } else {
        test_info.direction = (enum direction_e) direction_code;
    }

    size_t args[3];
    size_t lens[3] = {
            MAX_TEXT_LEN_TOKEN_LEN, NUM_ARGS_TOKEN_LEN, MAX_ARG_LEN_TOKEN_LEN
    };
    char token_names[3][MAX_TOKEN_LEN] = {
            MAX_TEXT_LEN_TOKEN, NUM_ARGS_TOKEN, MAX_ARG_LEN_TOKEN
    };

    for (size_t i = 0; i < 3; i++) {
        *line_size = getline(&line_buf, line_buf_size, file);
        if (*line_size < lens[i] || !starts_with(line_buf, token_names[i])) {
            test_info.success = false;
            return test_info;
        } else {
            char arg_text[*line_size - lens[i] + 1];
            snprintf(arg_text, sizeof(arg_text), "%*s", (int) (*line_size - lens[i]), line_buf + lens[i]);
            size_t arg = strtol(arg_text, NULL, 10);
            if (arg_text[0] != '0' && arg == 0) {
                test_info.success = false;
                return test_info;
            } else {
                args[i] = arg;
            }
        }
    }

    test_info.max_text_len = args[0];
    test_info.num_args = args[1];
    test_info.max_arg_len = args[2];

    return test_info;
}

case_info_t parse_entry_info(char* line_buf, size_t num_args) {

}

entry_t read_input_output(char* line_buf, size_t* line_buf_size, FILE* file, ssize_t* line_size, bool input) {

}

test_result_t conduct_test(char* input, char* output, enum cipher_e cipher, enum direction_e direction) {

}

void print_test_result(size_t count, size_t passed, enum cipher_e cipher, enum direction_e direction) {

}