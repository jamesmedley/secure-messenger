#include "tls_handler.h"

static void (*gui_update_callback)(const char *name, const char *message) = NULL;

void set_gui_update_callback(void (*callback)(const char *name, const char *message)) {
    gui_update_callback = callback;
}

void send_client_hello(char *destination_ip){ 
    char *client_hello = construct_client_hello();
    size_t message_len = 41;
    networkMessage(client_hello, message_len, destination_ip);
}

void send_server_hello(int session_id, char *destination_ip){
    char *server_hello = construct_server_hello(session_id);
    size_t message_len = 41;
    networkMessage(server_hello, message_len, destination_ip);
}

void send_client_key_exchange(const char *client_public_key, int public_key_length, char *destination_ip){
    char *client_key_exchange = construct_client_key_exchange(client_public_key, public_key_length);
    size_t message_len = public_key_length + 5;
    networkMessage(client_key_exchange, message_len, destination_ip);
}

void send_server_encrypted_handshake(const char *encrypted_symmetric_key, int key_length, char *destination_ip){
    char *server_encrypted_handshake = construct_server_encrypted_handshake(encrypted_symmetric_key, key_length);
    size_t message_len = key_length + 5;
    networkMessage(server_encrypted_handshake, message_len, destination_ip);
}

void send_message(const char *msg_content, int content_length, char *destination_ip){
    char *message = construct_message(msg_content, content_length);
    size_t message_len = content_length + 5;
    networkMessage(message, message_len, destination_ip);
}

void receive_client_hello(char *src_ip_address){ 
    // generate session_id
    int session_id = 1; // e.g.
    send_server_hello(session_id, src_ip_address);
}

void receive_server_hello(int session_id, char *src_ip_address){
    // generate RSA keys
    // send public key 
    const char *client_public_key = "foo";
    int public_key_length = 3;

    send_client_key_exchange(client_public_key, public_key_length, src_ip_address);
}

void receive_client_key_exchange(const char *client_public_key, int public_key_length, char *src_ip_address){
    // generate symmetric key
    //encrypt symmetric key with client_public_key
    const char *encrypted_symmetric_key = "bar";
    int key_length = 3;

    send_server_encrypted_handshake(encrypted_symmetric_key, key_length, src_ip_address);
}

void receive_server_encrypted_handshake(const char *encrypted_symmetric_key, int key_length, char *src_ip_address){
    // callback to gui to set state to session established
}

void receive_message(const char *msg_content, int content_length, char *src_ip_address){
    gui_update_callback(src_ip_address, msg_content);
}

void init_tls_handler(){
    call_client_hello(receive_client_hello);
    call_server_hello(receive_server_hello);
    call_client_key_exchange(receive_client_key_exchange);
    call_server_encrypted_handshake(receive_server_encrypted_handshake);
    call_message(receive_message);
}