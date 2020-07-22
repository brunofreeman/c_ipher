#ifndef __CIPHERS_H__
#define __CIPHERS_H__

/*
 * @param plaintext the text to encrypt
 * @contract caller is responsible for
 *           freeing the return
 * @return the plaintext encrypted with
 *         the Atbash cipher
 */
char* atbash_encrypt(const char* plaintext);

/*
 * @param ciphertext the text to decrypt
 * @contract caller is responsible for
 *           freeing the return
 * @return the ciphertext decrypted from
 *         the Atbash cipher
 */
char* atbash_decrypt(const char* ciphertext);

/*
 * @param plaintext the text to encrypt
 * @param shift how far to shift each letter
 * @contract caller is responsible for
 *           freeing the return
 * @return the plaintext encrypted with
 *         the Caesar cipher of the
 *         supplied shift
 */
char* caesar_encrypt(const char* plaintext, const int shift);

/*
 * @param ciphertext the text to decrypt
 * @param shift how far each letter was shifted
 * @contract caller is responsible for
 *           freeing the return
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
 * @contract caller is responsible for
 *           freeing the return
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
 * @contract caller is responsible for
 *           freeing the return
 * @return the ciphertext decrypted from
 *         the Vigenère cipher with
 *         supplied key
 */
char* vigenere_decrypt(const char* ciphertext, const char* key);

/*
 * @param ciphertext the text to encrypt
 * @param step the space between consecutive
 *             mappings
 * @param shift the shift common to all
 *              mappings
 * @contract step is coprime with the length
 *           of the alphabet, 26
 * @contract caller is responsible for
 *           freeing the return
 * @return the plaintext encrypted with
 *         the affine cipher with supplied
 *         step and shift
 */
char* affine_encrypt(const char* plaintext, const int step, const int shift);

/*
 * @param plaintext the text to decrypt
 * @param step the space between consecutive
 *             mappings
 * @param shift the shift common to all
 *              mappings
 * @contract step is coprime with the length
 *           of the alphabet, 26
 * @contract caller is responsible for
 *           freeing the return
 * @return the ciphertext decrypted from
 *         the affine cipher with supplied
 *         step and shift
 */
char* affine_decrypt(const char* ciphertext, const int step, const int shift);

#endif // __CIPHERS_H__
