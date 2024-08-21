#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ws2tcpip.h>
#include <time.h>

#pragma comment(lib, "Ws2_32.lib")

char* construct_client_hello();
char* construct_server_hello(int session_id);
char* construct_client_key_exchange(const char *client_public_key, int public_key_length);
char* construct_server_encrypted_handshake(const char *encrypted_symmetric_key, const char *iv, int key_length);
char* construct_message(const char *msg_content, int content_length);
