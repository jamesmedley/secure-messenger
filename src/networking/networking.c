#include "networking.h"
#include "message_decoding.h"

SOCKET sock;

void error_exit(const char *message) {
    perror(message);
    exit(EXIT_FAILURE);
}

unsigned __stdcall receiveMessages(void *arg) {
    SOCKET sock = *(SOCKET *)arg;
    struct sockaddr_in client_addr;
    int client_addr_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recvfrom(sock, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&client_addr, &client_addr_len);
        if (bytes_received == SOCKET_ERROR) {
            printf("recvfrom failed with error: %d\n", WSAGetLastError());
            continue;
        }


        char *message = (char *)malloc(bytes_received);
        if (message) {
            memcpy(message, buffer, bytes_received);
        }

        char ip_address[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &client_addr.sin_addr, ip_address, sizeof(ip_address));
        char *ip_address_copy = strdup(ip_address);

        decode_message_received(message, ip_address_copy);

        // Clean up
        free(message);
        free(ip_address_copy);
    }
    return 0;
}

void setupSocket(){
    WSADATA wsa;
    struct sockaddr_in server_addr;

    // Initialize Winsock
    if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
        error_exit("WSAStartup failed");
    }

    // Create a socket
    if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET) {
        error_exit("Socket creation failed");
    }

    // Setup the server address
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Bind the socket
    if (bind(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
        error_exit("Bind failed");
    }

    // Create a thread to listen for incoming messages
    _beginthreadex(NULL, 0, receiveMessages, (void *)&sock, 0, NULL);

}

int networkMessage(char *message, size_t message_len, char *dest_ip) {
    struct sockaddr_in dest_addr;
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);

    // Convert IPv4 address from text to binary form
    if (inet_pton(AF_INET, dest_ip, &dest_addr.sin_addr.s_addr) <= 0) {
        printf("Invalid address or Address not supported\n");
        return -1; 
    }

    // Send the message
    if (sendto(sock, message, message_len, 0, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) == SOCKET_ERROR) {
        printf("sendto failed with error\n");
        return -1;
    }

    return 0; // Success
}