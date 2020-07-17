#ifndef __CIPHERS_H__
#define __CIPHERS_H__

char* atbash_encrypt(const char* plaintext);

char* atbash_decrypt(const char* ciphertext);

char* caesar_encrypt(const char* plaintext, const int shift);

char* caesar_decrypt(const char* plaintext, const int shift);

#endif //__CIPHERS_H__
