#include "tls_handler.h"

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
    // send public key 
    const char *client_public_key = "foo";
    int public_key_length = 3;

    send_client_key_exchange(client_public_key, public_key_length, src_ip_address);
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

    // TODO: encrypt symmetric key with client_public_key
    new_session_callback(symmetric_key, iv, 32, src_ip_address); // set server session info
    send_server_encrypted_handshake(symmetric_key, iv, 32, src_ip_address);
    cleanup_openssl();
}

void receive_server_encrypted_handshake(char *encrypted_symmetric_key, char *iv, int key_length, char *src_ip_address){ // CLIENT
    
    // decrypt symmetric key with client private key
    new_session_callback(encrypted_symmetric_key, iv, key_length, src_ip_address);  // callback to gui to set state to session established
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