#include "rsa1.h"

#define RSA_KEY_BITS 2048

EVP_PKEY *generate_rsa_keypair() {
    EVP_PKEY *pkey = NULL;
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, NULL);
    
    if (!ctx) {
        fprintf(stderr, "Error creating context for key generation\n");
        return NULL;
    }

    if (EVP_PKEY_keygen_init(ctx) <= 0) {
        fprintf(stderr, "Error initializing key generation\n");
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    if (EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, RSA_KEY_BITS) <= 0) {
        fprintf(stderr, "Error setting RSA key length\n");
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    if (EVP_PKEY_keygen(ctx, &pkey) <= 0) {
        fprintf(stderr, "Error generating RSA key pair\n");
        EVP_PKEY_CTX_free(ctx);
        return NULL;
    }

    EVP_PKEY_CTX_free(ctx);
    return pkey;
}

/* Encrypt data with RSA public key */
int rsa_encrypt(EVP_PKEY *pkey, const unsigned char *plaintext, int plaintext_len, unsigned char *ciphertext) {
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, NULL);
    size_t outlen = EVP_PKEY_size(pkey);

    if (!ctx) {
        fprintf(stderr, "Error creating context for encryption\n");
        return -1;
    }

    if (EVP_PKEY_encrypt_init(ctx) <= 0) {
        fprintf(stderr, "Error initializing encryption\n");
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }

    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0) {
        fprintf(stderr, "Error setting RSA padding\n");
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }

    if (EVP_PKEY_encrypt(ctx, ciphertext, &outlen, plaintext, plaintext_len) <= 0) {
        fprintf(stderr, "RSA encryption failed\n");
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }

    EVP_PKEY_CTX_free(ctx);
    return (int)outlen;
}

/* Decrypt data with RSA private key */
int rsa_decrypt(EVP_PKEY *pkey, const unsigned char *ciphertext, int ciphertext_len, unsigned char *plaintext) {
    EVP_PKEY_CTX *ctx = EVP_PKEY_CTX_new(pkey, NULL);
    size_t outlen = EVP_PKEY_size(pkey);

    if (!ctx) {
        fprintf(stderr, "Error creating context for decryption\n");
        return -1;
    }

    if (EVP_PKEY_decrypt_init(ctx) <= 0) {
        fprintf(stderr, "Error initializing decryption\n");
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }

    if (EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) <= 0) {
        fprintf(stderr, "Error setting RSA padding\n");
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }

    if (EVP_PKEY_decrypt(ctx, plaintext, &outlen, ciphertext, ciphertext_len) <= 0) {
        fprintf(stderr, "RSA decryption failed\n");
        EVP_PKEY_CTX_free(ctx);
        return -1;
    }

    EVP_PKEY_CTX_free(ctx);
    return (int)outlen;
}

/* Serialize the public key to memory (PEM format) */
char *extract_public_key(EVP_PKEY *pkey) {
    BIO *bio = BIO_new(BIO_s_mem());
    char *pubkey_pem = NULL;

    if (!bio) {
        fprintf(stderr, "Error creating BIO for public key\n");
        return NULL;
    }

    if (PEM_write_bio_PUBKEY(bio, pkey) <= 0) {
        fprintf(stderr, "Error writing public key to BIO\n");
        BIO_free(bio);
        return NULL;
    }

    long pubkey_len = BIO_get_mem_data(bio, &pubkey_pem);
    if (pubkey_len <= 0) {
        fprintf(stderr, "Error getting public key from BIO\n");
        BIO_free(bio);
        return NULL;
    }

    char *pubkey_copy = malloc(pubkey_len + 1);
    if (!pubkey_copy) {
        fprintf(stderr, "Error allocating memory for public key\n");
        BIO_free(bio);
        return NULL;
    }

    memcpy(pubkey_copy, pubkey_pem, pubkey_len);
    pubkey_copy[pubkey_len] = '\0'; // null-terminate the string

    BIO_free(bio);
    return pubkey_copy;
}

EVP_PKEY *load_public_key_from_string(const char *pubkey_str) {
    BIO *bio = BIO_new_mem_buf(pubkey_str, -1);
    if (!bio) {
        fprintf(stderr, "Error creating BIO for public key\n");
        return NULL;
    }

    EVP_PKEY *pkey = PEM_read_bio_PUBKEY(bio, NULL, NULL, NULL);
    BIO_free(bio);

    if (!pkey) {
        fprintf(stderr, "Error loading public key from string\n");
        return NULL;
    }

    return pkey;
}

char *extract_private_key(EVP_PKEY *pkey) {
    BIO *bio = BIO_new(BIO_s_mem());
    char *privkey_pem = NULL;

    if (!bio) {
        fprintf(stderr, "Error creating BIO for private key\n");
        return NULL;
    }

    if (PEM_write_bio_PrivateKey(bio, pkey, NULL, NULL, 0, NULL, NULL) <= 0) {
        fprintf(stderr, "Error writing private key to BIO\n");
        BIO_free(bio);
        return NULL;
    }

    long privkey_len = BIO_get_mem_data(bio, &privkey_pem);
    if (privkey_len <= 0) {
        fprintf(stderr, "Error getting private key from BIO\n");
        BIO_free(bio);
        return NULL;
    }

    char *privkey_copy = malloc(privkey_len + 1);
    if (!privkey_copy) {
        fprintf(stderr, "Error allocating memory for private key\n");
        BIO_free(bio);
        return NULL;
    }

    memcpy(privkey_copy, privkey_pem, privkey_len);
    privkey_copy[privkey_len] = '\0'; // null-terminate the string

    BIO_free(bio);
    return privkey_copy;
}

/* Load the private key from a PEM string */
EVP_PKEY *load_private_key_from_string(const char *privkey_str) {
    BIO *bio = BIO_new_mem_buf(privkey_str, -1);
    if (!bio) {
        fprintf(stderr, "Error creating BIO for private key\n");
        return NULL;
    }

    EVP_PKEY *pkey = PEM_read_bio_PrivateKey(bio, NULL, NULL, NULL);
    BIO_free(bio);

    if (!pkey) {
        fprintf(stderr, "Error loading private key from string\n");
        return NULL;
    }

    return pkey;
}