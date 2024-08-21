#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/evp.h>
#include <stdio.h>
#include <string.h>
#include "crypto_utils.h"

#define KEY_LENGTH 2048
#define PUB_EXP 65537
#define PRINT_KEYS 1
#define WRITE_TO_FILE 0

/* Generate RSA key pair */
void generate_rsa_keypair(EVP_PKEY **pkey) {
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    if (!ctx) {
        // Handle error
    }
    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        // Handle error
    }
    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) <= 0) {
        // Handle error
    }
    if (EVP_PKEY_keygen(ctx, pkey) <= 0) {
        // Handle error
    }
    EVP_PKEY_CTX_free(ctx);
}

int rsa_encrypt(const unsigned char *plaintext, int plaintext_len, unsigned char *ciphertext, const char *pubkey_path) {
    EVP_PKEY *pubkey = EVP_PKEY_new();
    FILE *pubkey_file = fopen(pubkey_path, "r");
    if (!pubkey_file) return -1;

    PEM_read_PUBKEY(pubkey_file, &pubkey, NULL, NULL);
    fclose(pubkey_file);

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pubkey, NULL);
    if (!ctx) return -1;

    if (EVP_PKEY_encrypt_init(ctx) <= 0) return -1;
    size_t outlen;
    if (EVP_PKEY_encrypt(ctx, NULL, &outlen, plaintext, plaintext_len) <= 0) return -1;
    if (EVP_PKEY_encrypt(ctx, ciphertext, &outlen, plaintext, plaintext_len) <= 0) return -1;

    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(pubkey);
    return outlen;
}

int rsa_decrypt(const unsigned char *ciphertext, int ciphertext_len, unsigned char *plaintext, const char *privkey_path) {
    EVP_PKEY *privkey = EVP_PKEY_new();
    FILE *privkey_file = fopen(privkey_path, "r");
    if (!privkey_file) return -1;

    PEM_read_PrivateKey(privkey_file, &privkey, NULL, NULL);
    fclose(privkey_file);

    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(privkey, NULL);
    if (!ctx) return -1;

    if (EVP_PKEY_decrypt_init(ctx) <= 0) return -1;
    size_t outlen;
    if (EVP_PKEY_decrypt(ctx, NULL, &outlen, ciphertext, ciphertext_len) <= 0) return -1;
    if (EVP_PKEY_decrypt(ctx, plaintext, &outlen, ciphertext, ciphertext_len) <= 0) return -1;

    EVP_PKEY_CTX_free(ctx);
    EVP_PKEY_free(privkey);
    return outlen;
}

/* Utility function to print RSA keys */
void print_key(const char *label, const unsigned char *key) {
    printf("%s:\n%s\n", label, key);
}