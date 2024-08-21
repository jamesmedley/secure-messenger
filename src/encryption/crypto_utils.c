#include <openssl/evp.h>
#include <openssl/err.h>

void initialize_openssl() {
    OpenSSL_add_all_algorithms();
    ERR_load_crypto_strings();
}

void cleanup_openssl() {
    EVP_cleanup();
    ERR_free_strings();
}
