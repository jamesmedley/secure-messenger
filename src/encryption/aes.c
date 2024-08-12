#include <openssl/aes.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <string.h>

void generate_symmetric_key(unsigned char *key, size_t key_length) {
    if (RAND_bytes(key, key_length) != 1) {
        fprintf(stderr, "Error generating symmetric key\n");
    }
}

void aes_encrypt(const unsigned char *key, const unsigned char *data, unsigned char *encrypted, int *encrypted_len) {
    AES_KEY encrypt_key;
    AES_set_encrypt_key(key, 256, &encrypt_key);  // 256-bit key

    int len = ((strlen((const char*)data) / AES_BLOCK_SIZE) + 1) * AES_BLOCK_SIZE;
    AES_cbc_encrypt(data, encrypted, len, &encrypt_key, 1, AES_ENCRYPT); //TODO change IV (1)
    *encrypted_len = len;
}

void aes_decrypt(const unsigned char *key, const unsigned char *encrypted, unsigned char *decrypted, int encrypted_len) {
    AES_KEY decrypt_key;
    AES_set_decrypt_key(key, 256, &decrypt_key);  // 256-bit key

    AES_cbc_encrypt(encrypted, decrypted, encrypted_len, &decrypt_key, 1, AES_DECRYPT); //TODO change IV (1)
}
