#ifndef __CIPHERS_H__
#define __CIPHERS_H__

/*
 * Contracts shared by all functions:
 *
 * @contract plaintext/ciphertext must be null-terminated
 * @contract caller is responsible for
 *           freeing the return
 */

/*
 * @param plaintext the text to encrypt
 * @return the plaintext encrypted with
 *         the A1Z26 cipher
 */
char* a1z26_encrypt(const char* plaintext);

/*
 * @param ciphertext the text to decrypt
 * @return the ciphertext decrypted from
 *         the A1Z26 cipher
 */
char* a1z26_decrypt(const char* ciphertext);

/*
 * @param plaintext the text to encrypt
 * @param step the space between consecutive
 *             mappings
 * @param shift the shift common to all
 *              mappings
 * @contract step is coprime with the length
 *           of the alphabet, 26
 * @return the plaintext encrypted with
 *         the affine cipher with supplied
 *         step and shift
 */
char* affine_encrypt(const char* plaintext, const int step, const int shift);

/*
 * @param ciphertext the text to decrypt
 * @param step the space between consecutive
 *             mappings
 * @param shift the shift common to all
 *              mappings
 * @contract step is coprime with the length
 *           of the alphabet, 26
 * @return the ciphertext decrypted from
 *         the affine cipher with supplied
 *         step and shift
 */
char* affine_decrypt(const char* ciphertext, const int step, const int shift);

/*
 * @param plaintext the text to encrypt
 * @return the plaintext encrypted with
 *         the Atbash cipher
 */
char* atbash_encrypt(const char* plaintext);

/*
 * @param ciphertext the text to decrypt
 * @return the ciphertext decrypted from
 *         the Atbash cipher
 */
char* atbash_decrypt(const char* ciphertext);

/*
 * @param plaintext the text to encrypt
 * @return the plaintext encrypted with
 *         the Baconian cipher
 */
char* baconian_encrypt(const char* plaintext);

/*
 * @param ciphertext the text to decrypt
 * @return the ciphertext decrypted from
 *         the Baconian cipher
 */
char* baconian_decrypt(const char* ciphertext);

/*
 * @param plaintext the text to encrypt
 * @param shift how far to shift each letter
 * @return the plaintext encrypted with
 *         the Caesar cipher of the
 *         supplied shift
 */
char* caesar_encrypt(const char* plaintext, const int shift);

/*
 * @param ciphertext the text to decrypt
 * @param shift how far each letter was shifted
 * @return the ciphertext decrypted from
 *         the Caesar cipher of the
 *         supplied shift
 */
char* caesar_decrypt(const char* plaintext, const int shift);

/*
 * @param plaintext the text to encrypt
 * @param key the cipher key
 * @contract key consists of only
 *           uppercase letters
 * @return the plaintext encrypted with
 *         the Vigenère cipher and
 *         supplied key
 */
char* vigenere_encrypt(const char* plaintext, const char* key);

/*
 * @param ciphertext the text to decrypt
 * @param key the cipher key
 * @contract key consists of only
 *           uppercase letters
 * @return the ciphertext decrypted from
 *         the Vigenère cipher with
 *         supplied key
 */
char* vigenere_decrypt(const char* ciphertext, const char* key);

#endif // __CIPHERS_H__
