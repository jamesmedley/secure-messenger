#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")


void decode_message_received(const char *message, char *src_ip_address);

void call_client_hello(void (*callback)(char *src_ip_address, int seq_num, int session_id, char *client_random));
void call_server_hello(void (*callback)(char *src_ip_address, int seq_num, int session_id, char *server_random, char *server_public_key));
void call_client_key_exchange(void (*callback)(char *src_ip_address, int seq_num, char *encrypted_premaster));
void call_client_ready(void (*callback)(char *src_ip_address, int seq_num, char *handshake_hash));
void call_server_ready(void (*callback)(char *src_ip_address, int seq_num, char *handshake_hash));
void call_message(void (*callback)(const char *msg_content, int content_len, char *src_ip_address));
int charArrayToInt(const char *array, int startIndex);
