#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")


void decode_message_received(const char *message, char *src_ip_address);

void call_client_hello(void (*callback)(char *src_ip_address));
void call_server_hello(void (*callback)(int session_id, char *src_ip_address));
void call_client_key_exchange(void (*callback)(const char *client_public_key, int public_key_length, char *src_ip_address));
void call_server_encrypted_handshake(void (*callback)(const char *encrypted_symmetric_key, int key_length, char *src_ip_address));
void call_message(void (*callback)(const char *msg_content, int content_length, char *src_ip_address));