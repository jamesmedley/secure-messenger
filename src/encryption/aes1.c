#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/hmac.h>
#include <stdio.h>
#include <string.h>
#include "crypto_utils.h"

#define KEY_LEN 32

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


int generate_premaster(unsigned char *key){
    if (1 != RAND_bytes(key, 32)) {
        fprintf(stderr, "Error generating random bytes for key\n");
        cleanup_openssl();
        return 1;
    }
    return 0;
}

int generate_keys(unsigned char *key, unsigned char *iv){
    if (1 != RAND_bytes(key, 32)) {
        fprintf(stderr, "Error generating random bytes for key\n");
        cleanup_openssl();
        return 1;
    }
    if (1 != RAND_bytes(iv, 16)) {
        fprintf(stderr, "Error generating random bytes for iv\n");
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


int generate_symmetric_key(const unsigned char *client_random, const unsigned char *server_random,
                           const unsigned char *premaster_secret, unsigned char *symmetric_key, unsigned char *iv) { // TODO: make this work properly
    
    unsigned char combined_input[KEY_LEN * 3]; // 96 bytes (32B client_random + 32B server_random + 32B premaster_secret)

    // Combine client_random, server_random, and premaster_secret
    memcpy(combined_input, client_random, KEY_LEN);
    memcpy(combined_input + KEY_LEN, server_random, KEY_LEN);
    memcpy(combined_input + (KEY_LEN * 2), premaster_secret, KEY_LEN);

    // Use a KDF (e.g., HMAC-SHA256) to derive the symmetric key
    if (!HMAC(EVP_sha256(), premaster_secret, KEY_LEN, combined_input, sizeof(combined_input), symmetric_key, NULL)) {
        fprintf(stderr, "Error deriving symmetric key\n");
        return 1;
    }
    for(int i = 0; i<16; i++){
        char a = client_random[i];
        char b = server_random[i];
        char c = premaster_secret[i];
        iv[i] = a ^ b ^ c;
    }
    return 0;
}
