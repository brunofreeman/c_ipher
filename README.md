# c_ipher
`c_ipher` is a collection of ciphers implemented in C.
Thus far, the following ciphers are available:

1. [Atbash](https://en.wikipedia.org/wiki/Atbash)
2. [Caesar](https://en.wikipedia.org/wiki/Caesar_cipher)
3. [Vigenère](https://en.wikipedia.org/wiki/Vigen%C3%A8re_cipher)
4. [Affine](https://en.wikipedia.org/wiki/Affine_cipher)

The `c_ipher` executable can be used to encrypt and decrypt with the ciphers via the command line.
The `c_ipher_tests` executable can be used to run the tests defined in the `def` directory.
The included `CMakeLists.txt` builds both of these executables.