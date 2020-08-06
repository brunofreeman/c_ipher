# c_ipher
`c_ipher` is a collection of ciphers implemented in C.
Thus far, the following ciphers are available:

1. [Affine](https://en.wikipedia.org/wiki/Affine_cipher)
2. [Atbash](https://en.wikipedia.org/wiki/Atbash)
3. [Baconian](https://en.wikipedia.org/wiki/Bacon%27s_cipher)
4. [Caesar](https://en.wikipedia.org/wiki/Caesar_cipher)
5. [Vigenère](https://en.wikipedia.org/wiki/Vigen%C3%A8re_cipher)

## Executables

The `c_ipher` executable encrypts or decrypts with a specified cipher via the command line.
The `c_ipher_tests` executable runs the tests defined in the `test` directory.
The included `CMakeLists.txt` builds both of these executables.

## Test File Format
The test files are assumed to adhere to the following format.
`[]` notation denotes variable information.
`...` notation denotes a continuation of variable length.
Note the newline at the end of the file.
```text
#cipher=[cipher name]
#direction=[encrypt or decrypt]
???<[param 1.1]><[param 1.2]>...
[first input]
//--->
[first output]
!!!
???<[param 2.1]><[param 2.2]>...
[second input]
//--->
[second output]
!!!
...

```