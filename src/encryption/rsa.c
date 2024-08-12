#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <stdio.h>

RSA* generate_rsa_key(int bits) {
    RSA *rsa = RSA_new();
    BIGNUM *bne = BN_new();
    BN_set_word(bne, RSA_F4);  // RSA_F4 is the public exponent

    if (RSA_generate_key_ex(rsa, bits, bne, NULL) != 1) {
        ERR_print_errors_fp(stderr);
        RSA_free(rsa);
        rsa = NULL;
    }

    BN_free(bne);
    return rsa;
}

void save_rsa_key(RSA *rsa, const char *pub_key_file, const char *priv_key_file) {
    FILE *pub_file = fopen(pub_key_file, "wb");
    FILE *priv_file = fopen(priv_key_file, "wb");
    
    if (pub_file && priv_file) {
        PEM_write_RSAPublicKey(pub_file, rsa);
        PEM_write_RSAPrivateKey(priv_file, rsa, NULL, NULL, 0, NULL, NULL);
    }

    fclose(pub_file);
    fclose(priv_file);
}

int rsa_encrypt(const unsigned char *data, int data_len, unsigned char *encrypted, RSA *rsa) {
    int result = RSA_public_encrypt(data_len, data, encrypted, rsa, RSA_PKCS1_OAEP_PADDING);
    if (result == -1) {
        ERR_print_errors_fp(stderr);
    }
    return result;
}

int main() {
    RSA *rsa = generate_rsa_key(2048);  // Generate 2048-bit RSA key
    if (rsa) {
        save_rsa_key(rsa, "client_public.pem", "client_private.pem");
        RSA_free(rsa);
    }
    return 0;
}
