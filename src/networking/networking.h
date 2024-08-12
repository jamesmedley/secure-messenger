#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>

#pragma comment(lib, "Ws2_32.lib")

#define BUFFER_SIZE 1024
#define PORT 8888

void setupSocket();
int networkMessage(char *message, size_t message_len, char *dest_ip);