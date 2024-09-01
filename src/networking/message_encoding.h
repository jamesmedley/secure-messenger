#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ws2tcpip.h>
#include <time.h>

#pragma comment(lib, "Ws2_32.lib")

char* construct_client_hello();
char* construct_server_hello(int session_id, int seq_num, const char *server_public_key, int public_key_length);
char* construct_client_key_exchange(const char *encrypted_premaster, int seq_num);
char* construct_client_ready(const char *handshake_hash, int seq_num);
char* construct_server_ready(const char *handshake_hash, int seq_num);
char* construct_message(const char *msg_content, int content_length);
