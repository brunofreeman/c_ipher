#ifndef __VALIDATE_H__
#define __VALIDATE_H__

#include <stdbool.h>

/*
 * @param text the text to check
 * @return true if the text contains
 *         only uppercase letters,
 *         false otherwise
 */
bool is_only_upper_alpha(const char* text);

/*
 * @return true if a and b are coprime,
 *         false otherwise
 */
bool are_coprime(const int a, const int b);

#endif // __VALIDATE_H__
