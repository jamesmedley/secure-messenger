void initialize_openssl();
void cleanup_openssl();
int encrypt_aes(const unsigned char *key, const unsigned char *iv,
                const unsigned char *plaintext, int plaintext_len,
                unsigned char *ciphertext);
int decrypt_aes(const unsigned char *key, const unsigned char *iv,
                const unsigned char *ciphertext, int ciphertext_len,
                unsigned char *plaintext);
int generate_premaster(unsigned char *key);
int generate_keys(unsigned char *key, unsigned char *iv);
int generate_symmetric_key(const unsigned char *client_random, const unsigned char *server_random,
                           const unsigned char *premaster_secret, unsigned char *symmetric_key, unsigned char *iv);
void print_hex(const char *label, const unsigned char *data, int len);