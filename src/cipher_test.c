#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "ciphers.h"
#include "validate.h"

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
#define ARGS_TOKEN_LEN 3
#define ARGS_LEFT_DELIM '<'
#define ARGS_RIGHT_DELIM '>'

#define IN_TO_OUT_TOKEN "//--->\n"
#define IN_TO_OUT_TOKEN_LEN 7

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

test_info_t read_test_info(char* line_buf, size_t* line_buf_size, FILE* file, ssize_t* line_size);

case_info_t parse_case_info(char* line_buf/*, size_t num_args, size_t max_arg_len*/);

entry_t read_input_output(char* line_buf, size_t* line_buf_size, FILE* file, ssize_t* line_size/*, size_t max_size*/, bool input);

test_result_t conduct_test(char* input, char* output, enum cipher_e cipher, enum direction_e direction, char** args, size_t num_args);

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

        // char*  input = malloc((test_info.max_text_len + 1) * sizeof(char));
        // char* output = malloc((test_info.max_text_len + 1) * sizeof(char));

        enum read_mode_e read_mode = ENTRY_INFO;

        case_info_t case_info;
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
                    case_info = parse_case_info(line_buf/*, test_info.num_args, test_info.max_arg_len*/);
                    if (!case_info.success) {
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
                output_read = read_input_output(line_buf, &line_buf_size, file, &line_size, true);
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
                        if (test_result.passed) tests_passed++;
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
        // free(input);
        // free(output);

        if (read_mode == FAILURE) return EXIT_FAILURE;
        else print_test_result(test_count, tests_passed, test_info.cipher, test_info.direction);
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

test_info_t read_test_info(char* line_buf, size_t* line_buf_size, FILE* file, ssize_t* line_size) {
    test_info_t test_info;
    test_info.success = false;

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
        return test_info;
    } else {
        test_info.direction = (enum direction_e) direction_code;
    }

    /*size_t args[3];
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
    test_info.max_arg_len = args[2];*/

    test_info.success = true;
    return test_info;
}

case_info_t parse_case_info(char* line_buf/*, size_t num_args, size_t max_arg_len*/) {
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

    while (line_buf[line_idx]) {
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

    size_t line_buf_idx = ARGS_TOKEN_LEN;
    size_t args_idx = 0;
    size_t arg_text_idx = 0;

    while (line_buf[line_buf_idx]) {
        /*if (prev_delim == ' ') {
            if (c == ARGS_LEFT_DELIM) {
                prev_delim = ARGS_RIGHT_DELIM;
                continue;
            } else {
                return case_info;
            }
        }*/
        char other_delim = prev_delim == ARGS_RIGHT_DELIM ? ARGS_LEFT_DELIM : ARGS_RIGHT_DELIM;

        if (line_buf[line_buf_idx] == other_delim) {
            prev_delim = other_delim;
            if (prev_delim == ARGS_RIGHT_DELIM) {
                args_idx++;
                arg_text_idx = 0;
            }
        } else if (line_buf[line_buf_idx] == prev_delim) {
            return case_info;
        } else {
            args[args_idx][arg_text_idx++] = line_buf[line_buf_idx];
        }

        /*switch (c) {
            case ARGS_LEFT_DELIM:
                if (prev_delim == ARGS_RIGHT_DELIM) {
                    prev_delim = ARGS_LEFT_DELIM;
                } else {
                    return case_info;
                }
                prev_delim = ARGS_LEFT_DELIM;
                break;
            case ARGS_RIGHT_DELIM:
                if (prev_delim == ARGS_LEFT_DELIM) {
                    prev_delim = ARGS_RIGHT_DELIM;
                    args_idx++;
                } else {
                    return case_info;
                }
                break;
            default:
                args[args_idx][arg_text_idx++] = c;
        }*/
    }

    case_info.args = args;
    case_info.num_args = num_args;

    case_info.success = true;
    return case_info;
}

entry_t read_input_output(char* line_buf, size_t* line_buf_size, FILE* file, ssize_t* line_size/*, size_t max_size*/, bool input) {
    entry_t entry;
    entry.success = false;

    char* text = malloc(sizeof(char));
    text[0] = '\0';
    size_t text_len = 1;

    bool reading = true;
    while (reading) {
        *line_size = getline(&line_buf, line_buf_size, file);
        if (input && starts_with(line_buf, IN_TO_OUT_TOKEN)) {
            if (*line_size != IN_TO_OUT_TOKEN_LEN) return entry;
            reading = false;
        } else if (!input && (starts_with(line_buf, ARGS_TOKEN) || line_size == EOF)) {
            reading = false;
        } else {
            text = realloc(text, (text_len + *line_size) * sizeof(char));
            if (!text) return entry;
            strcpy(text + text_len, line_buf);
            text_len += *line_size;
            text[text_len - 1] = '\0';
        }
    }

    entry.entry = text;
    entry.success = true;
    return entry;
}

test_result_t conduct_test(char* input, char* output, enum cipher_e cipher, enum direction_e direction, char** args, size_t num_args) {
    test_result_t test_result;
    test_result.success = false;

    char* correct_output = NULL;

    switch (cipher) {
        case AFFINE: {
            if (num_args != 2) return test_result;

            int step = (int) strtol(args[0], NULL, 10);
            if ((args[0][0] != '0' && step == 0) || !are_coprime(step, 26)) return test_result;

            int shift = (int) strtol(args[1], NULL, 10);
            if (args[1][0] != '0' && shift == 0) return test_result;

            correct_output = direction == ENCRYPTION ?
                    affine_encrypt(input, step, shift) : affine_decrypt(input, step, shift);
            break;
        } case ATBASH: {
            if (num_args != 0) return test_result;
            correct_output = direction == ENCRYPTION ?
                    atbash_encrypt(input) : atbash_decrypt(input);
            break;
        } case CAESAR: {
            if (num_args != 1) return test_result;

            int shift = (int) strtol(args[0], NULL, 10);
            if (args[0][0] != '0' && shift == 0) return test_result;

            correct_output = direction == ENCRYPTION ?
                    caesar_encrypt(input, shift) : caesar_decrypt(input, shift);
            break;
        } case VIGENERE: {
            if (num_args != 1) return test_result;

            if (!is_only_upper_alpha(args[0])) return test_result;

            correct_output = direction == ENCRYPTION ?
                    vigenere_encrypt(input, args[0]) : vigenere_decrypt(input, args[0]);
            break;
        }
    }

    test_result.passed = strcmp(output, correct_output) == 0;
    free(correct_output);

    test_result.success = true;
    return test_result;
}

void print_test_result(size_t count, size_t passed, enum cipher_e cipher, enum direction_e direction) {
    printf("%s ", passed == count ? "PASSED" : "FAILED");
    switch (cipher) {
        case AFFINE:
            printf("%8s %s: ", "affine",  direction == ENCRYPTION ? "encryption" : "decryption");
            break;
        case ATBASH:
            printf("%8s %s: ", "atbash",  direction == ENCRYPTION ? "encryption" : "decryption");
            break;
        case CAESAR:
            printf("%8s %s: ", "caesar",  direction == ENCRYPTION ? "encryption" : "decryption");
            break;
        case VIGENERE:
            printf("%8s %s: ", "vigenere", direction == ENCRYPTION ? "encryption" : "decryption");
            break;
    }
    printf("%3zu/%3zu (%3f)\n", passed, count, passed / (float) count);
}