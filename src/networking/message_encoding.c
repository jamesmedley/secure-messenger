#include "message_encoding.h"

// Function to add integer to char array in network byte order
void addIntToCharArray(char *array, int value, int startIndex) {
    value = htonl(value);  // Convert to network byte order
    memcpy(&array[startIndex], &value, sizeof(value));
}


char* construct_client_hello() {
    char msg_type = 0x01;  // Client Hello
    int msg_content_length = 36;

    // Allocate memory for the message
    char *message = (char *)malloc(1 + 4 + 32 + 4);  // MSG_TYPE + MSG_CONTENT_LENGTH + CLIENT_RANDOM + SESSIONID
    if (message == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Generate CLIENT_RANDOM
    char client_random[32];
    int timestamp = (int)time(NULL);
    addIntToCharArray(client_random, timestamp, 0);  // Add timestamp
    for (int i = 4; i < 32; ++i) {
        client_random[i] = rand() % 256;  // Add random bytes
    }

    // Generate SESSIONID
    int session_id = rand();

    // Construct the message
    message[0] = msg_type;
    addIntToCharArray(message, msg_content_length, 1);  // Add content length
    memcpy(&message[5], client_random, 32);  // Add client random
    addIntToCharArray(message, session_id, 37);  // Add session id

    return message;
}

char* construct_server_hello(int session_id) {
    char msg_type = 0x02;  // Server Hello
    int msg_content_length = 36;

    // Allocate memory for the message
    char *message = (char *)malloc(1 + 4 + 32 + 4);  // MSG_TYPE + MSG_CONTENT_LENGTH + CLIENT_RANDOM + SESSIONID
    if (message == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Generate CLIENT_RANDOM
    char server_random[32];
    int timestamp = (int)time(NULL);
    addIntToCharArray(server_random, timestamp, 0);  // Add timestamp
    for (int i = 4; i < 32; ++i) {
        server_random[i] = rand() % 256;  // Add random bytes
    }

    // Construct the message
    message[0] = msg_type;
    addIntToCharArray(message, msg_content_length, 1);  // Add content length
    memcpy(&message[5], server_random, 32);  // Add client random
    addIntToCharArray(message, session_id, 37);  // Add session id

    return message;
}

char* construct_client_key_exchange(const char *client_public_key, int public_key_length) {
    char msg_type = 0x03;  // Client Key Exchange

    // Allocate memory for the message
    char *message = (char *)malloc(1 + 4 + public_key_length);  // MSG_TYPE + MSG_CONTENT_LENGTH + CLIENT_PUBLIC_KEY
    if (message == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Construct the message
    message[0] = msg_type;
    addIntToCharArray(message, public_key_length, 1);  // Add content length
    memcpy(&message[5], client_public_key, public_key_length);  // Add client public key

    return message;
}

char* construct_server_encrypted_handshake(const char *encrypted_symmetric_key, int key_length) {
    char msg_type = 0x04;  // Server Encrypted Handshake

    // Allocate memory for the message
    char *message = (char *)malloc(1 + 4 + key_length);  // MSG_TYPE + MSG_CONTENT_LENGTH + ENCRYPTED_SYMMETRIC_KEY
    if (message == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Construct the message
    message[0] = msg_type;
    addIntToCharArray(message, key_length, 1);  // Add content length
    memcpy(&message[5], encrypted_symmetric_key, key_length);  // Add encrypted symmetric key

    return message;
}

char* construct_message(const char *msg_content, int content_length) {
    char msg_type = 0x05;  // Message

    // Allocate memory for the message, including space for the null terminator
    char *message = (char *)malloc(1 + 4 + content_length);  // MSG_TYPE + MSG_CONTENT_LENGTH + MSG_CONTENT
    if (message == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Construct the message
    message[0] = msg_type;
    addIntToCharArray(message, content_length, 1);  // Add content length
    memcpy(&message[5], msg_content, content_length);  // Add message content

    return message;
}
