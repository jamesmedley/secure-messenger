#include "tls_handler.h"

EVP_PKEY *rsa_keypair;

static void (*gui_update_callback)(const char *name, const char *message, int message_len) = NULL;
void set_gui_update_callback(void (*callback)(const char *name, const char *message, int message_len)) {
    gui_update_callback = callback;
}

static void (*new_session_callback)(char *encrypted_symmetric_key, char *iv, int key_length, char *src_ip_address) = NULL;
void set_new_session_callback(void (*callback)(char *encrypted_symmetric_key, char *iv, int key_length, char *src_ip_address)) {
    new_session_callback = callback;
}

static void (*connection_req_callback)(char *src_ip_address) = NULL;
void set_connection_req_callback(void (*callback)(char *src_ip_address)) {
    connection_req_callback = callback;
}


void send_client_hello(char *destination_ip){ // CLIENT
    char *client_hello = construct_client_hello();
    size_t message_len = 41;
    networkMessage(client_hello, message_len, destination_ip);
    free(client_hello);
}

void send_server_hello(int session_id, char *destination_ip){ // SERVER
    char *server_hello = construct_server_hello(session_id); 
    size_t message_len = 41;
    networkMessage(server_hello, message_len, destination_ip);
    free(server_hello);
}

void send_client_key_exchange(const char *client_public_key, int public_key_length, char *destination_ip){ // CLIENT
    char *client_key_exchange = construct_client_key_exchange(client_public_key, public_key_length);
    size_t message_len = public_key_length + 5;
    networkMessage(client_key_exchange, message_len, destination_ip);
    free(client_key_exchange);
}
void send_server_encrypted_handshake(char *encrypted_symmetric_key, char *iv, int key_length, char *destination_ip){ // SERVER
    char *server_encrypted_handshake = construct_server_encrypted_handshake(encrypted_symmetric_key, iv, key_length);
    size_t message_len = 5 + key_length + 16;
    networkMessage(server_encrypted_handshake, message_len, destination_ip);
    free(server_encrypted_handshake);
}

void send_message(const char *msg_content, int content_length, char *destination_ip){ // CLIENT/SERVER
    char *message = construct_message(msg_content, content_length);
    size_t message_len = content_length + 5;
    networkMessage(message, message_len, destination_ip);
    free(message);
}

void receive_client_hello(char *src_ip_address){ // SERVER
    

    // send connection request ... if accept: send_server_hello(session_id, src_ip_address);
    connection_req_callback(src_ip_address);

}

void receive_server_hello(int session_id, char *src_ip_address){ // CLIENT
    // generate RSA keys
    rsa_keypair = generate_rsa_keypair();
    if (!rsa_keypair) {
        fprintf(stderr, "Key generation failed\n");
        return;
    }

    const char *public_key = extract_public_key(rsa_keypair);
    if (!public_key) {
        fprintf(stderr, "Error extracting public key\n");
        EVP_PKEY_free(rsa_keypair);
        return;
    }
    // send public key 
    
    int public_key_length = strlen(public_key);

    send_client_key_exchange(public_key, public_key_length, src_ip_address);
}

void receive_client_key_exchange(const char *client_public_key, int public_key_length, char *src_ip_address){ // SERVER
    // generate symmetric key
    unsigned char symmetric_key[32];
    unsigned char iv[16];
    initialize_openssl();
    if (generate_keys(symmetric_key, iv) != 0) {
        fprintf(stderr, "Key generation failed\n");
        cleanup_openssl();
        return;
    }
    
    EVP_PKEY *public_key = load_public_key_from_string(client_public_key);
    if (!public_key) {
        fprintf(stderr, "Error loading public key from string\n");
        free((void *)client_public_key);
        return;
    }

    unsigned char ciphertext[256];
    int ciphertext_len = rsa_encrypt(public_key, (const unsigned char *)symmetric_key, 32, ciphertext);

    if (ciphertext_len == -1) {
        fprintf(stderr, "RSA encryption failed\n");
        free(public_key);
        EVP_PKEY_free(public_key);
        return;
    }

    new_session_callback(symmetric_key, iv, 32, src_ip_address); // set server session info
    send_server_encrypted_handshake(ciphertext, iv, 256, src_ip_address);
    cleanup_openssl();
    EVP_PKEY_free(public_key);
}

void receive_server_encrypted_handshake(char *encrypted_symmetric_key, char *iv, int key_length, char *src_ip_address){ // CLIENT
    
    char *private_key_str = extract_private_key(rsa_keypair);
    if (!private_key_str) {
        fprintf(stderr, "Error extracting keys\n");
        EVP_PKEY_free(rsa_keypair);
        return;
    }


    EVP_PKEY *private_key = load_private_key_from_string(private_key_str);
    if (!private_key) {
        fprintf(stderr, "Error loading private key from string\n");
        free(private_key_str);
        EVP_PKEY_free(rsa_keypair);
        return;
    }

    unsigned char decrypted_key[256];
    int decryptedtext_len = rsa_decrypt(private_key, encrypted_symmetric_key, key_length, decrypted_key); // broke
    
    free(private_key_str);
    EVP_PKEY_free(private_key);
    EVP_PKEY_free(rsa_keypair);
    // decrypt symmetric key with client private key
    new_session_callback(decrypted_key, iv, key_length, src_ip_address);  // callback to gui to set state to session established
}

void receive_message(const char *msg_content, int content_len, char *src_ip_address){ // CLIENT/SERVER
    gui_update_callback(src_ip_address, msg_content, content_len);
}

void init_tls_handler(){
    call_client_hello(receive_client_hello);
    call_server_hello(receive_server_hello);
    call_client_key_exchange(receive_client_key_exchange);
    call_server_encrypted_handshake(receive_server_encrypted_handshake);
    call_message(receive_message);
}