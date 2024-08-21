#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/rsa.h>
#include <openssl/rand.h>
#include <stdio.h>
#include <string.h>

EVP_PKEY *generate_rsa_keypair();
int rsa_encrypt(EVP_PKEY *pkey, const unsigned char *plaintext, int plaintext_len, unsigned char *ciphertext);
int rsa_decrypt(EVP_PKEY *pkey, const unsigned char *ciphertext, int ciphertext_len, unsigned char *plaintext);
void print_openssl_error();
char *extract_public_key(EVP_PKEY *pkey);
char *extract_private_key(EVP_PKEY *pkey);
EVP_PKEY *load_public_key_from_string(const char *pubkey_str);
EVP_PKEY *load_private_key_from_string(const char *privkey_str);