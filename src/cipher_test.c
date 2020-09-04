#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "ciphers.h"
#include "validate.h"

#define NUM_TESTS 10
#define TEST_NAME_LEN 17
#define NUM_CIPHERS 5
#define CIPHER_NAME_LEN 9

#define CIPHER_NAMES_LIST "affine", "atbash", "baconian", "caesar", "vigenere"
#define TEST_NAMES_LIST \
      "affine_encrypt",   "affine_decrypt", \
      "atbash_encrypt",   "atbash_decrypt", \
    "baconian_encrypt", "baconian_decrypt", \
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
#define CIPHER_TOKEN_LEN 8
#define DIRECTION_TOKEN "#direction="
#define DIRECTION_TOKEN_LEN 11

#define ARGS_TOKEN "???"
#define ARGS_TOKEN_LEN 3
#define IN_TO_OUT_TOKEN "//--->\n"
#define IN_TO_OUT_TOKEN_LEN 7
#define CASE_END_TOKEN "!!!\n"
#define CASE_END_TOKEN_LEN 4

#define ARGS_LEFT_DELIM '<'
#define ARGS_RIGHT_DELIM '>'

enum read_mode_e {
    ENTRY_INFO = 0,
    INPUT      = 1,
    OUTPUT     = 2,
    FAILURE    = 3
};

enum cipher_e {
    AFFINE   = 0,
    ATBASH   = 1,
    BACONIAN = 2,
    CAESAR   = 3,
    VIGENERE = 4
};

enum direction_e {
    ENCRYPTION = 0,
    DECRYPTION = 1
};

/*
 * @member args needs to be freed
 */
typedef struct {
    char** args;
    size_t num_args;
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
    bool success;
} test_info_t;

typedef struct {
    bool passed;
    bool success;
} test_result_t;

bool starts_with(const char* text, const char* start);

test_info_t read_test_info(char** line_buf, size_t* line_buf_size, FILE** file, ssize_t* line_size);

case_info_t parse_case_info(const char* line_buf);

entry_t read_input_output(char** line_buf, size_t* line_buf_size, FILE** file, ssize_t* line_size, bool input);

test_result_t conduct_test(const char* input, const char* output, enum cipher_e cipher, enum direction_e direction, char** args, size_t num_args);

void print_test_result(size_t count, size_t passed, size_t chars_in, size_t chars_out, enum cipher_e cipher, enum direction_e direction);

int main(int argc, char** argv) {

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

        test_info_t test_info = read_test_info(&line_buf, &line_buf_size, &file, &line_size);
        if (!test_info.success) {
            fclose(file);
            free(line_buf);
            return EXIT_FAILURE;
        }

        enum read_mode_e read_mode = ENTRY_INFO;

        case_info_t case_info;
        entry_t input_read;
        entry_t output_read;

        size_t test_count = 0;
        size_t tests_passed = 0;
        size_t chars_in = 0;
        size_t chars_out = 0;

        bool reading = true;

        while (reading) switch (read_mode) {
            case ENTRY_INFO: {
                line_size = getline(&line_buf, &line_buf_size, file);
                if (line_size == EOF) {
                    reading = false;
                } else {
                    case_info = parse_case_info(line_buf);
                    if (!case_info.success) {
                        read_mode = FAILURE;
                        reading = false;
                    } else {
                        read_mode = INPUT;
                    }
                }
                break;
            } case INPUT: {
                input_read = read_input_output(&line_buf, &line_buf_size, &file, &line_size, true);
                if (!input_read.success) {
                    for (size_t j = 0; j < case_info.num_args; j++) {
                        free(case_info.args[j]);
                    }
                    free(case_info.args);
                    read_mode = FAILURE;
                    reading = false;
                } else {
                    read_mode = OUTPUT;
                }
                break;
            } case OUTPUT: {
                output_read = read_input_output(&line_buf, &line_buf_size, &file, &line_size, false);
                if (!output_read.success) {
                    for (size_t j = 0; j < case_info.num_args; j++) {
                        free(case_info.args[j]);
                    }
                    free(case_info.args);
                    free(input_read.entry);
                    read_mode = FAILURE;
                    reading = false;
                } else {
                    test_result_t test_result =
                            conduct_test(
                                    input_read.entry,
                                    output_read.entry,
                                    test_info.cipher,
                                    test_info.direction,
                                    case_info.args,
                                    case_info.num_args);
                    if (!test_result.success) {
                        read_mode = FAILURE;
                        reading = false;
                    } else {
                        test_count++;
                        tests_passed += test_result.passed;
                        chars_in += strlen(input_read.entry);
                        chars_out += strlen(output_read.entry);
                        read_mode = ENTRY_INFO;
                    }
                    for (size_t j = 0; j < case_info.num_args; j++) {
                        free(case_info.args[j]);
                    }
                    free(case_info.args);
                    free(input_read.entry);
                    free(output_read.entry);
                }
                break;
            } default: {
                read_mode = FAILURE;
                reading = false;
            }
        }

        fclose(file);
        free(line_buf);

        if (read_mode == FAILURE) return EXIT_FAILURE;
        else print_test_result(test_count, tests_passed, chars_in, chars_out, test_info.cipher, test_info.direction);
    }

    return EXIT_SUCCESS;
}

bool starts_with(const char* text, const char* start) {
    size_t idx = 0;
    while (text[idx] && start[idx]) {
        if (text[idx] != start[idx]) return false;
        idx++;
    }
    return !start[idx];
}

test_info_t read_test_info(char** line_buf, size_t* line_buf_size, FILE** file, ssize_t* line_size) {
    test_info_t test_info;
    test_info.success = false;

    *line_size = getline(&*line_buf, line_buf_size, *file);

    char cipher_names[NUM_CIPHERS][CIPHER_NAME_LEN] = {
            CIPHER_NAMES_LIST
    };

    int cipher_code = -1;

    for (size_t i = 0; i < NUM_CIPHERS; i++) {
        char potential_token[CIPHER_TOKEN_LEN + CIPHER_NAME_LEN + 1 + 1];
        snprintf(potential_token, sizeof(potential_token), "%s%s\n", CIPHER_TOKEN, cipher_names[i]);
        if (strcmp(*line_buf, potential_token) == 0) {
            cipher_code = i;
            break;
        }
    }
    if (cipher_code == -1) {
        return test_info;
    } else {
        test_info.cipher = (enum cipher_e) cipher_code;
    }

    *line_size = getline(&*line_buf, line_buf_size, *file);

    char direction_names[NUM_DIRECTIONS][DIRECTION_NAME_LEN] = {
            DIRECTIONS_NAME_LIST
    };

    int direction_code = -1;

    for (size_t i = 0; i < NUM_DIRECTIONS; i++) {
        char potential_token[DIRECTION_TOKEN_LEN + DIRECTION_NAME_LEN + 1 + 1];
        snprintf(potential_token, sizeof(potential_token), "%s%s\n", DIRECTION_TOKEN, direction_names[i]);
        if (strcmp(*line_buf, potential_token) == 0) {
            direction_code = i;
            break;
        }
    }
    if (direction_code == -1) {
        return test_info;
    } else {
        test_info.direction = (enum direction_e) direction_code;
    }

    test_info.success = true;
    return test_info;
}

case_info_t parse_case_info(const char* line_buf) {
    case_info_t case_info;
    case_info.success = false;

    if (!starts_with(line_buf, ARGS_TOKEN)) {
        return case_info;
    }

    char prev_delim = ARGS_RIGHT_DELIM;
    size_t line_idx = ARGS_TOKEN_LEN;
    size_t num_args = 0;

    while (line_buf[line_idx]) {
        char other_delim = prev_delim == ARGS_RIGHT_DELIM ? ARGS_LEFT_DELIM : ARGS_RIGHT_DELIM;
        if (line_buf[line_idx] == other_delim) {
            prev_delim = other_delim;
            if (prev_delim == ARGS_RIGHT_DELIM) num_args++;
        } else if (line_buf[line_idx] == prev_delim) {
            return case_info;
        }
        line_idx++;
    }

    size_t arg_lens[num_args];
    for (size_t i = 0; i < num_args; i++) arg_lens[i] = 0;
    size_t arg_lens_idx = 0;
    line_idx = ARGS_TOKEN_LEN;

    while (line_buf[line_idx] && line_buf[line_idx] != '\n') {
        char other_delim = prev_delim == ARGS_RIGHT_DELIM ? ARGS_LEFT_DELIM : ARGS_RIGHT_DELIM;
        if (line_buf[line_idx] == other_delim) {
            prev_delim = other_delim;
            if (prev_delim == ARGS_RIGHT_DELIM) arg_lens_idx++;
        } else if (line_buf[line_idx] == prev_delim) {
            return case_info;
        } else {
            arg_lens[arg_lens_idx]++;
        }
        line_idx++;
    }

    char** args = malloc(num_args * sizeof(char*));
    for (size_t i = 0; i < num_args; i++) {
        args[i] = malloc(arg_lens[i] * sizeof(char));
    }

    line_idx = ARGS_TOKEN_LEN;
    size_t args_idx = 0;
    size_t arg_text_idx = 0;

    while (line_buf[line_idx] && line_buf[line_idx] != '\n') {
        char other_delim = prev_delim == ARGS_RIGHT_DELIM ? ARGS_LEFT_DELIM : ARGS_RIGHT_DELIM;

        if (line_buf[line_idx] == other_delim) {
            prev_delim = other_delim;
            if (prev_delim == ARGS_RIGHT_DELIM) {
                args_idx++;
                arg_text_idx = 0;
            }
        } else if (line_buf[line_idx] == prev_delim) {
            for (size_t j = 0; j < num_args; j++) {
                free(args[j]);
            }
            free(args);
            return case_info;
        } else {
            args[args_idx][arg_text_idx++] = line_buf[line_idx];
        }

        line_idx++;
    }

    case_info.args = args;
    case_info.num_args = num_args;

    case_info.success = true;
    return case_info;
}

entry_t read_input_output(char** line_buf, size_t* line_buf_size, FILE** file, ssize_t* line_size/*, size_t max_size*/, bool input) {
    entry_t entry;
    entry.success = false;

    char* text = malloc(sizeof(char));
    text[0] = '\0';
    size_t text_len = 1;

    bool reading = true;
    while (reading) {
        *line_size = getline(&*line_buf, line_buf_size, *file);
        if (input && starts_with(*line_buf, IN_TO_OUT_TOKEN)) {
            if (*line_size != IN_TO_OUT_TOKEN_LEN) {
                free(text);
                return entry;
            }
            reading = false;
        } else if (!input && starts_with(*line_buf, CASE_END_TOKEN)) {
            if (*line_size != CASE_END_TOKEN_LEN) {
                free(text);
                return entry;
            }
            reading = false;
        } else {
            char* new_text = realloc(text, (text_len + *line_size) * sizeof(char));
            if (!new_text) {
                free(text);
                return entry;
            } else {
                text = new_text;
            }
            strcpy(text + (text_len - 1), *line_buf);
            text_len += *line_size;
            text[text_len - 1] = '\0';
        }
    }

    entry.entry = text;
    entry.success = true;
    return entry;
}

test_result_t conduct_test(const char* input, const char* output, const enum cipher_e cipher, const enum direction_e direction, char** args, const size_t num_args) {
    test_result_t test_result;
    test_result.success = false;

    char* test_output = NULL;

    switch (cipher) {
        case AFFINE: {
            if (num_args != 2) return test_result;

            int step = (int) strtol(args[0], NULL, 10);
            if ((args[0][0] != '0' && step == 0) || !are_coprime(step, 26)) return test_result;

            int shift = (int) strtol(args[1], NULL, 10);
            if (args[1][0] != '0' && shift == 0) return test_result;

            test_output = direction == ENCRYPTION ?
                          affine_encrypt(input, step, shift) : affine_decrypt(input, step, shift);
            break;
        } case ATBASH: {
            if (num_args != 0) return test_result;
            test_output = direction == ENCRYPTION ?
                          atbash_encrypt(input) : atbash_decrypt(input);
            break;
        } case BACONIAN: {
            if (num_args != 0) return test_result;
            test_output = direction == ENCRYPTION ?
                          baconian_encrypt(input) : baconian_decrypt(input);
            break;
        } case CAESAR: {
            if (num_args != 1) return test_result;

            int shift = (int) strtol(args[0], NULL, 10);
            if (args[0][0] != '0' && shift == 0) return test_result;

            test_output = direction == ENCRYPTION ?
                    caesar_encrypt(input, shift) : caesar_decrypt(input, shift);
            break;
        } case VIGENERE: {
            if (num_args != 1) return test_result;

            if (!is_only_upper_alpha(args[0])) return test_result;

            test_output = direction == ENCRYPTION ?
                          vigenere_encrypt(input, args[0]) : vigenere_decrypt(input, args[0]);
            break;
        }
    }

    test_result.passed = strcmp(output, test_output) == 0;

    if (!test_result.passed) {
        printf("Failed case with:\n\t>       in = %s\t> true out = %s\t> test out = %s", input, output, test_output);
    }

    free(test_output);

    test_result.success = true;
    return test_result;
}

void print_test_result(const size_t count, const size_t passed, size_t chars_in, size_t chars_out, const enum cipher_e cipher, const enum direction_e direction) {
    printf("%s ", passed == count ? "PASSED" : "FAILED");
    char cipher_names[NUM_CIPHERS][CIPHER_NAME_LEN] = {
            CIPHER_NAMES_LIST
    };
    printf("%8s %s: ", cipher_names[cipher],   direction == ENCRYPTION ? "encryption" : "decryption");
    printf("%2zu/%-2zu (%5.1f%%) tests passed, %6zu chars ", passed, count, passed / (float) count * 100, chars_in);
    printf(direction == ENCRYPTION ? "encrypted" : "decrypted");
    printf(" into %6zu chars\n", chars_out);
}
