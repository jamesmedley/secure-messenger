#include "message_decoding.h"


static void (*receive_client_hello)(char *src_ip_address) = NULL;
void call_client_hello(void (*callback)(char *src_ip_address)) {
    receive_client_hello = callback;
}

static void (*receive_server_hello)(int session_id, char *src_ip_address) = NULL;
void call_server_hello(void (*callback)(int session_id, char *src_ip_address)) {
    receive_server_hello = callback;
}

static void (*receive_client_key_exchange)(const char *client_public_key, int public_key_length, char *src_ip_address) = NULL;
void call_client_key_exchange(void (*callback)(const char *client_public_key, int public_key_length, char *src_ip_address)) {
    receive_client_key_exchange = callback;
}

static void (*receive_server_encrypted_handshake)(const char *encrypted_symmetric_key, int key_length, char *src_ip_address) = NULL;
void call_server_encrypted_handshake(void (*callback)(const char *encrypted_symmetric_key, int key_length, char *src_ip_address)) {
    receive_server_encrypted_handshake = callback;
}

static void (*receive_message)(const char *msg_content, int content_length, char *src_ip_address) = NULL;
void call_message(void (*callback)(const char *msg_content, int content_length, char *src_ip_address)) {
    receive_message = callback;
}



// Function to convert bytes from network byte order to host byte order
int charArrayToInt(const char *array, int startIndex) {
    int value;
    memcpy(&value, &array[startIndex], sizeof(value));
    return ntohl(value);
}

// Function to decode a Client Hello message
void decode_client_hello(const char *message, char *src_ip_address) {
    int msg_content_length = charArrayToInt(message, 1);
    char client_random[32];
    memcpy(client_random, &message[5], 32);
    int session_id = charArrayToInt(message, 37);

    printf("Client Hello:\n");
    printf("Content Length: %d\n", msg_content_length);
    printf("Client Random: ");
    for (int i = 0; i < 32; ++i) {
        printf("%02x ", (unsigned char)client_random[i]);
    }
    printf("\nSession ID: %d\n", session_id);

    receive_client_hello(src_ip_address);

}

// Function to decode a Server Hello message
void decode_server_hello(const char *message, char *src_ip_address) {
    int msg_content_length = charArrayToInt(message, 1);
    char server_random[32];
    memcpy(server_random, &message[5], 32);
    int session_id = charArrayToInt(message, 37);

    printf("Server Hello:\n");
    printf("Content Length: %d\n", msg_content_length);
    printf("Server Random: ");
    for (int i = 0; i < 32; ++i) {
        printf("%02x ", (unsigned char)server_random[i]);
    }
    printf("\nSession ID: %d\n", session_id);

    receive_server_hello(session_id, src_ip_address);

}

// Function to decode a Client Key Exchange message
void decode_client_key_exchange(const char *message, char *src_ip_address) {
    int msg_content_length = charArrayToInt(message, 1);
    char *client_public_key = (char *)malloc(msg_content_length);
    memcpy(client_public_key, &message[5], msg_content_length);

    printf("Client Key Exchange:\n");
    printf("Content Length: %d\n", msg_content_length);
    printf("Client Public Key: ");
    for (int i = 0; i < msg_content_length; ++i) {
        printf("%02x ", (unsigned char)client_public_key[i]);
    }
    printf("\n");

    receive_client_key_exchange(client_public_key, msg_content_length, src_ip_address);

    free(client_public_key);
}

// Function to decode a Server Encrypted Handshake message
void decode_server_encrypted_handshake(const char *message_str, char *src_ip_address) {
    int msg_content_length = charArrayToInt(message_str, 1);
    char *encrypted_symmetric_key = (char *)malloc(msg_content_length);
    memcpy(encrypted_symmetric_key, &message_str[5], msg_content_length);

    printf("Server Encrypted Handshake:\n");
    printf("Content Length: %d\n", msg_content_length);
    printf("Encrypted Symmetric Key: ");
    for (int i = 0; i < msg_content_length; ++i) {
        printf("%02x ", (unsigned char)encrypted_symmetric_key[i]);
    }
    printf("\n");

    receive_server_encrypted_handshake(encrypted_symmetric_key, msg_content_length, src_ip_address);

    free(encrypted_symmetric_key);
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
            decode_server_encrypted_handshake(message, src_ip_address);
            break;
        case 0x05:
            decode_message(message, src_ip_address);
            break;
        default:
            printf("Unknown message type: %02x\n", (unsigned char)msg_type);
            break;
    }
}