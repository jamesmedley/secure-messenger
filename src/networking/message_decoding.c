#include "message_decoding.h"


static void (*receive_client_hello)(char *src_ip_address, int seq_num, int session_id, char *client_random) = NULL;
void call_client_hello(void (*callback)(char *src_ip_address, int seq_num, int session_id, char *client_random)) {
    receive_client_hello = callback;
}

static void (*receive_server_hello)(char *src_ip_address, int seq_num, int session_id, char *server_random, char *server_public_key) = NULL;
void call_server_hello(void (*callback)(char *src_ip_address, int seq_num, int session_id, char *server_random, char *server_public_key)) {
    receive_server_hello = callback;
}

static void (*receive_client_key_exchange)(char *src_ip_address, int seq_num, char *encrypted_premaster) = NULL;
void call_client_key_exchange(void (*callback)(char *src_ip_address, int seq_num, char *encrypted_premaster)) {
    receive_client_key_exchange = callback;
}

static void (*receive_client_ready)(char *src_ip_address, int seq_num, char *handshake_hash) = NULL;
void call_client_ready(void (*callback)(char *src_ip_address, int seq_num, char *handshake_hash)) {
    receive_client_ready = callback;
}

static void (*receive_server_ready)(char *src_ip_address, int seq_num, char *handshake_hash) = NULL;
void call_server_ready(void (*callback)(char *src_ip_address, int seq_num, char *handshake_hash)) {
    receive_server_ready = callback;
}

static void (*receive_message)(const char *msg_content, int content_len, char *src_ip_address) = NULL;
void call_message(void (*callback)(const char *msg_content, int content_len, char *src_ip_address)) {
    receive_message = callback;
}

static void (*receive_session_accept)(char *src_ip_address) = NULL;
void call_session_accept(void (*callback)(char *src_ip_address)) {
    receive_session_accept = callback;
}



// Function to convert bytes from network byte order to host byte order
int charArrayToInt(const char *array, int startIndex) {
    int value;
    memcpy(&value, &array[startIndex], sizeof(value));
    return ntohl(value);
}

// Function to decode a Client Hello message
void decode_client_hello(const char *message, char *src_ip_address) {
    int sequence_number = charArrayToInt(message, 1);  
    int msg_content_length = charArrayToInt(message, 5); 
    char client_random[32];
    memcpy(client_random, &message[9], 32); 
    int session_id = charArrayToInt(message, 41); 
    receive_client_hello(src_ip_address, sequence_number, session_id, client_random); 
}

// Function to decode a Server Hello message
void decode_server_hello(const char *message, char *src_ip_address) {
    int sequence_number = charArrayToInt(message, 1);  
    int msg_content_length = charArrayToInt(message, 5); 
    char server_random[32];
    memcpy(server_random, &message[9], 32); 
    int session_id = charArrayToInt(message, 41); 

    int public_key_len = msg_content_length-36;
    char server_public_key[public_key_len];
    memcpy(server_public_key, &message[45], public_key_len); 

    receive_server_hello(src_ip_address, sequence_number, session_id, server_random, server_public_key);

}

// Function to decode a Client Key Exchange message
void decode_client_key_exchange(const char *message, char *src_ip_address) {
    int sequence_number = charArrayToInt(message, 1);  
    int msg_content_length = charArrayToInt(message, 5); 
    char *encrypted_premaster = (char *)malloc(msg_content_length);
    memcpy(encrypted_premaster, &message[9], msg_content_length);

    receive_client_key_exchange(src_ip_address, sequence_number, encrypted_premaster);

    free(encrypted_premaster);
}

// Function to decode a client ready message
void decode_client_ready(const char *message, char *src_ip_address) {
    int sequence_number = charArrayToInt(message, 1);  
    int msg_content_length = charArrayToInt(message, 5); 
    char *handshake_hash = (char *)malloc(msg_content_length);
    memcpy(handshake_hash, &message[9], msg_content_length);
    receive_client_ready(src_ip_address, sequence_number, handshake_hash);

    free(handshake_hash);
}

// Function to decode a server ready message
void decode_server_ready(const char *message, char *src_ip_address) {
    int sequence_number = charArrayToInt(message, 1);  
    int msg_content_length = charArrayToInt(message, 5); 
    char *handshake_hash = (char *)malloc(msg_content_length);
    memcpy(handshake_hash, &message[9], msg_content_length);
    receive_server_ready(src_ip_address, sequence_number, handshake_hash);

    free(handshake_hash);
}


// Function to decode a generic message
void decode_message(const char *message, char *src_ip_address) {

    int msg_content_length = charArrayToInt(message, 1);

    if (msg_content_length <= 0) {
        printf("Invalid content length!\n");
        return;
    }

    char *msg_content = (char *)malloc(msg_content_length + 1); // +1 doe null terminator
    if (msg_content == NULL) {
        perror("Failed to allocate memory for message content");
        exit(EXIT_FAILURE);
    }

    memcpy(msg_content, &message[5], msg_content_length);
    msg_content[msg_content_length] = '\0';

    receive_message(msg_content, msg_content_length, src_ip_address);

    free(msg_content);
}


// Main decoding function
void decode_message_received(const char *message, char *src_ip_address) {
    char msg_type = message[0];
    switch (msg_type) {
        case 0x01:
            decode_client_hello(message, src_ip_address);
            break;
        case 0x02:
            decode_server_hello(message, src_ip_address);
            break;
        case 0x03:
            decode_client_key_exchange(message, src_ip_address);
            break;
        case 0x04:
            decode_client_ready(message, src_ip_address);        
            break;
        case 0x05:
            decode_server_ready(message, src_ip_address);        
            break;
        case 0x06:
            decode_message(message, src_ip_address);
            break;
        default:
            printf("Unknown message type: %02x\n", (unsigned char)msg_type);
            break;
    }
}