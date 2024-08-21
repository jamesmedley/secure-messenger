#include <openssl/evp.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <string.h>
#include "crypto_utils.h"

/* Encrypt data with AES-256-CBC */
int encrypt_aes(const unsigned char *key, const unsigned char *iv,
                const unsigned char *plaintext, int plaintext_len,
                unsigned char *ciphertext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int ciphertext_len;

    if (!(ctx = EVP_CIPHER_CTX_new())) return -1;

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) return -1;

    if (1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len)) return -1;
    ciphertext_len = len;

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) return -1;
    ciphertext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return ciphertext_len;
}

/* Decrypt data with AES-256-CBC */
int decrypt_aes(const unsigned char *key, const unsigned char *iv,
                const unsigned char *ciphertext, int ciphertext_len,
                unsigned char *plaintext) {
    EVP_CIPHER_CTX *ctx;
    int len;
    int plaintext_len;

    if (!(ctx = EVP_CIPHER_CTX_new())) return -1;

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv)) return -1;

    if (1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len)) return -1;
    plaintext_len = len;

    if (1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) return -1;
    plaintext_len += len;

    EVP_CIPHER_CTX_free(ctx);

    return plaintext_len;
}


int generate_keys(unsigned char *key, unsigned char *iv){
    if (1 != RAND_bytes(key, 32)) {
        fprintf(stderr, "Error generating random bytes for key\n");
        cleanup_openssl();
        return 1;
    }
    if (1 != RAND_bytes(iv, 16)) {
        fprintf(stderr, "Error generating random bytes for IV\n");
        cleanup_openssl();
        return 1;
    }
    return 0;
}



/* Utility function to print bytes in hex format */
void print_hex(const char *label, const unsigned char *data, int len) {
    printf("%s: ", label);
    for (int i = 0; i < len; i++) {
        printf("%02x", data[i]);
    }
    printf("\n");
}
