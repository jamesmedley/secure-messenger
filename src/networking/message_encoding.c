#include "message_encoding.h"

// Function to add integer to char array in network byte order
void addIntToCharArray(char *array, int value, int startIndex) {
    value = htonl(value);  // Convert to network byte order
    memcpy(&array[startIndex], &value, sizeof(value));
}


char* construct_client_hello() {
    char msg_type = 0x01;  // Client Hello
    int seq_num = 1;  // Sequence number set to 1
    int msg_content_length = 36;

    // Allocate memory for the message
    char *message = (char *)malloc(1 + 4 + 4 + 32 + 4);  // MSG_TYPE + SEQUENCE_NUMBER + MSG_CONTENT_LENGTH + CLIENT_RANDOM + SESSIONID
    if (message == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Generate CLIENT_RANDOM
    char client_random[32];
    int timestamp = (int)time(NULL);
    addIntToCharArray(client_random, timestamp, 0);  // Add timestamp
    for (int i = 4; i < 32; ++i) {
        client_random[i] = rand() % 256;  // Add random bytes TODO: change seeding
    }

    // Generate SESSIONID
    int session_id = rand();

    message[0] = msg_type;
    addIntToCharArray(message, seq_num, 1);  // Add sequence number (starting at index 1)
    addIntToCharArray(message, msg_content_length, 5);  // Add content length (starting at index 5)
    memcpy(&message[9], client_random, 32);  // Add client random (starting at index 9)
    addIntToCharArray(message, session_id, 41);  // Add session id (starting at index 41)

    return message;
}

char* construct_server_hello(int session_id, int seq_num, const char *server_public_key, int public_key_length) {
    char msg_type = 0x02;  // Server Hello
    int msg_content_length = 32 + 4 + public_key_length;

    // Allocate memory for the message
    char *message = (char *)malloc(1 + 4 + 4 + msg_content_length);  // MSG_TYPE + SEQUENCE_NUMBER + MSG_CONTENT_LENGTH + CLIENT_RANDOM + SESSIONID + SRV_PUBLIC_KEY
    if (message == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Generate CLIENT_RANDOM
    char server_random[32];
    int timestamp = (int)time(NULL);
    addIntToCharArray(server_random, timestamp, 0);  // Add timestamp
    for (int i = 4; i < 32; ++i) {
        server_random[i] = rand() % 256;  // Add random bytes TODO: change seeding
    }

    // Construct the message
    message[0] = msg_type;
    addIntToCharArray(message, seq_num, 1);  // Add sequence number (starting at index 1)
    addIntToCharArray(message, msg_content_length, 5);  // Add content length (starting at index 5)
    memcpy(&message[9], server_random, 32);  // Add client random (starting at index 9)
    addIntToCharArray(message, session_id, 41);  // Add session id (starting at index 41)
    memcpy(&message[45], server_public_key, public_key_length);  // Add server public key

    return message;
}

char* construct_client_key_exchange(const char *encrypted_premaster, int seq_num) {
    char msg_type = 0x03;  // Client Key Exchange
    int msg_content_length = 256;

    // Allocate memory for the message
    char *message = (char *)malloc(1 + 4 + 4 + msg_content_length);  // MSG_TYPE + SEQUENCE_NUMBER + MSG_CONTENT_LENGTH + CLIENT_PUBLIC_KEY
    if (message == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Construct the message
    message[0] = msg_type;
    addIntToCharArray(message, seq_num, 1);  // Add sequence number (starting at index 1)
    addIntToCharArray(message, msg_content_length, 5);  // Add content length (starting at index 5)
    memcpy(&message[9], encrypted_premaster, msg_content_length);

    return message;
}

char* construct_client_ready(const char *handshake_hash, int seq_num){
    char msg_type = 0x04;  // Client Key Exchange
    int msg_content_length = strlen(handshake_hash); // TODO: change length when implement hash function
    // Allocate memory for the message
    char *message = (char *)malloc(1 + 4 + 4 + msg_content_length);  // MSG_TYPE + SEQUENCE_NUMBER + MSG_CONTENT_LENGTH + HASH_DIGEST
    if (message == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Construct the message
    message[0] = msg_type;
    addIntToCharArray(message, seq_num, 1);  // Add sequence number (starting at index 1)
    addIntToCharArray(message, msg_content_length, 5);  // Add content length (starting at index 5)
    memcpy(&message[9], handshake_hash, msg_content_length);

    return message;
}

char* construct_server_ready(const char *handshake_hash, int seq_num){
    char msg_type = 0x05;  // Client Key Exchange
    int msg_content_length = strlen(handshake_hash); // TODO: change length when implement hash function
    // Allocate memory for the message
    char *message = (char *)malloc(1 + 4 + 4 + msg_content_length);  // MSG_TYPE + SEQUENCE_NUMBER + MSG_CONTENT_LENGTH + HASH_DIGEST
    if (message == NULL) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }

    // Construct the message
    message[0] = msg_type;
    addIntToCharArray(message, seq_num, 1);  // Add sequence number (starting at index 1)
    addIntToCharArray(message, msg_content_length, 5);  // Add content length (starting at index 5)
    memcpy(&message[9], handshake_hash, msg_content_length);

    return message;
}

char* construct_message(const char *msg_content, int content_length) {
    char msg_type = 0x06;  // Message

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
