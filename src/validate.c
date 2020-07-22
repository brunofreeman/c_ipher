#include <stdlib.h>
#include "validate.h"

/*
 * @param dividend the number to take the
 *        modulus of
 * @param divisor the modulus
 */
static int mod(const int dividend, const int divisor) {
    return ((dividend % divisor) + divisor) % divisor;
}

/*
 * Euclid's algorithm
 * @return the greatest common divisor of
 *         a and b
 */
static int gcd(int a, int b) {
    int temp;
    while (b != 0) {
        temp = b;
        b = mod(a, b);
        a = temp;
    }
    return a;
}

bool is_only_upper_alpha(const char* text) {
    size_t idx = 0;
    char curr = text[0];
    while (curr) {
        if (curr < 'A' || 'Z' < curr) return false;
        curr = text[++idx];
    }
    return true;
}

bool are_coprime(const int a, const int b) {
    return gcd(a, b) == 1;
}
