/*
 * Copyright (c) 2020 Alex Spataru <https://github.com/alex-spataru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "Crypto.h"

/*
 * Quick and dirty min() implementation
 */
#define min(x,y) x > y ? x : y

/*
 * Define ASCII alphabet for Caesar cypher
 */
#define ALPHABET_START  33
#define ALPHABET_END    126
#define ALPHABET_SIZE   ALPHABET_END - ALPHABET_START

/**
 * @brief Crypto::encryptData
 * @param data
 * @param key
 * @param error
 * @return
 *
 * Encrypts the given @a data in the following manner:
 *   -# With the length of the key, we execute a Caesar cypher on printable characters of the
 *      given @a data
 *   -# With the given key, we execute a XOR cypher on the output of the Caesar cypher
 */
QByteArray Crypto::encryptData(const QByteArray& data, const QByteArray& key, CryptoError* error)
{
    // Validate arguments
    Q_ASSERT(error);

    // Validate key length
    if(key.isEmpty()) {
        *error = kPasswordEmpty;
        return data;
    }

    // Init. variables & reset error state
    QByteArray caesar;
    QByteArray xorData;
    *error = kNoError;

    // Execute Caesar cipher with length of key over whole ASCII table
    {
        // Get key length
        int k = min(key.length(), ALPHABET_SIZE);

        // Rotate each individual byte of the data
        for (int i = 0; i < data.length(); ++i) {
            // Get byte (we use int to handle ASCII-table overflow)
            int byte = static_cast<int>(data.at(i));

            // Only encrypt alphabet characters
            if (byte >= ALPHABET_START && byte <= ALPHABET_END) {
                // Move byte
                byte += k;

                // Ensure byte is valid ASCII character
                if (byte > ALPHABET_END)
                    byte = byte - ALPHABET_END + ALPHABET_START - 1;
            }

            // Add obtained byte to encrypted data array
            caesar.append(static_cast<char>(byte));
        }
    }

    // Execute XOR cipher on Caesar output
    int k = key.length();
    for (int i = 0; i < caesar.length(); ++i) {
        char byte = caesar.at(i);
        byte ^= key.at(i % k);
        xorData.append(byte);
    }

    // Return output
    return xorData;
}

/**
 * @brief Crypto::decryptData
 * @param data
 * @param key
 * @param error
 * @return
 *
 * Decyphers the given @a data in the following order:
 *   -# We execute a XOR cypher between the @a data and the @a key to obtain the original
 *      Caesar cypher
 *   -# We execute inverse Caesar algorithm to obtain original data
 */
QByteArray Crypto::decryptData(const QByteArray& data, const QByteArray& key, CryptoError* error)
{
    // Validate arguments
    Q_ASSERT(error);

    // Validate key length
    if(key.isEmpty()) {
        *error = kPasswordEmpty;
        return data;
    }

    // Init. variables & reset error state
    QByteArray caesar;
    QByteArray xorData;
    *error = kNoError;

    // Execute XOR cipher on encrypted data to decrypt
    int k = key.length();
    for (int i = 0; i < data.length(); ++i) {
        char byte = data.at(i);
        byte ^= key.at(i % k);
        xorData.append(byte);
    }

    // Caesar De-cipher
    {
        // Get key length
        int k = min(key.length(), ALPHABET_SIZE);

        // Special cases (obtained while testing crypto/decrypt)
        // Restore original position of each byte
        for (int i = 0; i < xorData.length(); ++i) {
            // Get byte (use int. to manage cases where the byte has rotated the entire table)
            int byte = static_cast<int>(xorData.at(i));

            // Only decipher alphabet characters
            if (byte >= ALPHABET_START && byte <= ALPHABET_END) {
                // Move byte to original position
                byte -= k;

                // Ensure byte is valid ASCII character
                if (byte < ALPHABET_START)
                    byte = byte + ALPHABET_END - ALPHABET_START + 1;
            }

            // Add obtained byte to decrypted data array
            caesar.append(static_cast<char>(byte));
        }
    }

    // Return obtained data
    return caesar;
}
