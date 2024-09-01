#include "tls_handler.h"

EVP_PKEY *rsa_keypair;

HandshakeData *handshake_client;
HandshakeData *handshake_server;
KeyData *key_data_client;
KeyData *key_data_server;

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
    size_t message_len = 45;
    
    handshake_client = malloc(sizeof(HandshakeData));
    handshake_client -> curr_seq_num = charArrayToInt(client_hello, 1);
    char client_random[32];
    memcpy(client_random, &client_hello[9], 32);
    handshake_client -> client_random = strdup(client_random);
    handshake_client -> session_id = charArrayToInt(client_hello, 41);
    
    networkMessage(client_hello, message_len, destination_ip);
    free(client_hello);
}

void send_server_hello(int session_id, char *destination_ip){ // SERVER
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
    
    char *server_hello = construct_server_hello(session_id, (handshake_server->curr_seq_num)+1, public_key, public_key_length); //increment seq_num
    size_t message_len = 1 + 4 + 4 + 32 + 4 + public_key_length;

    char server_random[32];
    memcpy(server_random, &server_hello[9], 32);
    handshake_server -> server_random = strdup(server_random);

    networkMessage(server_hello, message_len, destination_ip);
    free(server_hello);
}

void send_client_key_exchange(const char *encrypted_premaster, char *destination_ip){ // CLIENT
    char *client_key_exchange = construct_client_key_exchange(encrypted_premaster, (handshake_client->curr_seq_num)+1);
    size_t message_len = 256 + 9;
    networkMessage(client_key_exchange, message_len, destination_ip);
    free(client_key_exchange);
}

void send_client_ready(const char *handshake_hash, char *destination_ip){
    char *ready = construct_client_ready(handshake_hash, 1); // TODO: change seq num to correct handshake data
    size_t message_len = 9 + strlen(handshake_hash); // TODO: change length when implement hash function
    networkMessage(ready, message_len, destination_ip);
    free(ready);
}

void send_server_ready(const char *handshake_hash, char *destination_ip){
    char *ready = construct_server_ready(handshake_hash, 1); // TODO: change seq num to correct handshake data
    size_t message_len = 9 + strlen(handshake_hash); // TODO: change length when implement hash function
    networkMessage(ready, message_len, destination_ip);
    free(ready);
}

void send_message(const char *msg_content, int content_length, char *destination_ip){ // CLIENT/SERVER
    char *message = construct_message(msg_content, content_length);
    size_t message_len = content_length + 5;
    networkMessage(message, message_len, destination_ip);
    free(message);
}

void receive_client_hello(char *src_ip_address, int seq_num, int session_id, char *client_random){ // SERVER
    handshake_server = malloc(sizeof(HandshakeData));
    handshake_server -> curr_seq_num = seq_num;
    handshake_server -> session_id = session_id;
    handshake_server -> client_random = strdup(client_random);

    // send connection request ... if accept: send_server_hello(session_id, src_ip_address);
    connection_req_callback(src_ip_address);

}

void receive_server_hello(char *src_ip_address, int seq_num, int session_id, char *server_random, char *server_public_key){ // CLIENT
    // generate premaster secret
    unsigned char premaster_secret[32];
    initialize_openssl();
    if (generate_premaster(premaster_secret) != 0) {
        fprintf(stderr, "Key generation failed\n");
        cleanup_openssl();
        return;
    }
    
    handshake_client -> premaster_secret = strdup(premaster_secret);
    handshake_client -> server_random = strdup(server_random);
    handshake_client -> curr_seq_num = seq_num;
    
    EVP_PKEY *public_key = load_public_key_from_string(server_public_key);
    if (!public_key) {
        fprintf(stderr, "Error loading public key from string\n");
        free((void *)server_public_key);
        return;
    }

    unsigned char encrypted_premaster[256];
    int ciphertext_len = rsa_encrypt(public_key, (const unsigned char *)premaster_secret, 32, encrypted_premaster);


    if (ciphertext_len == -1) {
        fprintf(stderr, "RSA encryption failed\n");
        free(public_key);
        EVP_PKEY_free(public_key);
        return;
    }

    send_client_key_exchange(encrypted_premaster, src_ip_address);


    // client generate symmetric key 
    key_data_client = malloc(sizeof(KeyData));
    key_data_client->iv = malloc(16);
    key_data_client->key_length = 32;
    key_data_client->symmetric_key = malloc(32); 
    generate_symmetric_key(handshake_client->client_random, handshake_client->server_random, handshake_client->premaster_secret, key_data_client->symmetric_key, key_data_client->iv);
    
    // TODO: send client ready
    send_client_ready("example handshake hash", src_ip_address);
    
    cleanup_openssl();
}

void receive_client_key_exchange(char *src_ip_address, int seq_num, char *encrypted_premaster){ // SERVER
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

    unsigned char premaster[32];
    int decryptedtext_len = rsa_decrypt(private_key, encrypted_premaster, 256, premaster);

    free(private_key_str);
    EVP_PKEY_free(private_key);
    EVP_PKEY_free(rsa_keypair);

    handshake_server -> premaster_secret = strdup(premaster);
    handshake_server -> curr_seq_num = seq_num;

    // server generate symmetric key 
    key_data_server = malloc(sizeof(KeyData));
    key_data_server->iv = malloc(16);
    key_data_server->key_length = 32;
    key_data_server->symmetric_key = malloc(32); 
    generate_symmetric_key(handshake_server->client_random, handshake_server->server_random, handshake_server->premaster_secret, key_data_server->symmetric_key, key_data_server->iv);
    
    // TODO: send server ready
    send_server_ready("example handshake hash", src_ip_address);

}

void receive_client_ready(char *src_ip_address, int seq_num, char *handshake_hash){ // SERVER
    // compare hashes
    
    // if hashes match:
    new_session_callback(strdup(key_data_server->symmetric_key), strdup(key_data_server->iv), key_data_server->key_length, src_ip_address);
    free(key_data_server->iv);
    free(key_data_server->symmetric_key);
    free(key_data_server);
    free(handshake_server->client_random);
    free(handshake_server->server_random);
    free(handshake_server->premaster_secret);
    free(handshake_server);
}

void receive_server_ready(char *src_ip_address, int seq_num, char *handshake_hash){ // CLIENT
    // compare hashes
    
    // if hashes match:
    new_session_callback(strdup(key_data_client->symmetric_key), strdup(key_data_client->iv), key_data_client->key_length, src_ip_address);
    free(key_data_client->iv);
    free(key_data_client->symmetric_key);
    free(key_data_client);
    free(handshake_client->client_random);
    free(handshake_client->server_random);
    free(handshake_client->premaster_secret);
    free(handshake_client);
}

void receive_message(const char *msg_content, int content_len, char *src_ip_address){ // CLIENT/SERVER
    gui_update_callback(src_ip_address, msg_content, content_len);
}

void init_tls_handler(){
    call_client_hello(receive_client_hello);
    call_server_hello(receive_server_hello);
    call_client_key_exchange(receive_client_key_exchange);
    call_client_ready(receive_client_ready);
    call_server_ready(receive_server_ready);
    call_message(receive_message);
}