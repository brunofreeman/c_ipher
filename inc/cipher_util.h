#ifndef __CIPHER_UTIL_H__
#define __CIPHER_UTIL_H__

#include <stdbool.h>

/*
 * @param text the text to check
 * @return true if the text contains
 *         only uppercase letters,
 *         false otherwise
 */
bool only_upper_alpha(const char* text);

/*
 * @TODO: not tested
 * @param text the basis text
 * @contract caller is responsible for
 *           freeing the return
 * @return text stripped of all non-
 *         alpha characters and all
 *         lowercase characters converted
 *         to uppercase
 */
// char* to_upper_alpha(const char* text)

#endif // __CIPHER_UTIL_H__
