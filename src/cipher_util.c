#include <stdlib.h>
#include "cipher_util.h"

bool only_upper_alpha(const char* text) {
    size_t idx = 0;
    char curr = text[0];
    while (curr) {
        if (curr < 'A' || 'Z' < curr) return false;
        curr = text[++idx];
    }
    return true;
}

/*char* to_upper_alpha(const char* text) {
    size_t len = 0;
    size_t idx = 0;
    char curr = text[0];
    while (curr) {
        bool upper = 'A' <= curr && curr <= 'Z';
        bool lower = 'a' <= curr && curr <= 'z';
        len += upper || lower;
        curr = text[++idx];
    }
    char* upper_alpha = malloc(len * sizeof(char));
    curr = text[0];
    idx = 0;
    size_t idx_ua = 0;
    while (curr) {
        bool upper = 'A' <= curr && curr <= 'Z';
        bool lower = 'a' <= curr && curr <= 'z';
        if (upper) upper_alpha[idx_ua++] = text[idx++];
        else if (lower) upper_alpha[idx_ua++] = text[idx++] - 'a' + 'A';
        else curr = text[++idx];
    }
    return upper_alpha;
}*/
