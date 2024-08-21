#include <openssl/rand.h>
#include <string.h>
#include <stdio.h>
#include "message_decoding.h"
#include "message_encoding.h"
#include "networking.h"
#include "../encryption/aes1.h"
#include "../encryption/crypto_utils.h"
#include "../encryption/rsa1.h"



void set_gui_update_callback(void (*callback)(const char *name, const char *message, int message_len));
void set_new_session_callback(void (*callback)(char *encrypted_symmetric_key, char *iv, int key_length, char *src_ip_address));
void set_connection_req_callback(void (*callback)(char *src_ip_address));
void send_message(const char *msg_content, int content_length, char *destination_ip);
void send_client_hello(char *destination_ip);
void send_server_hello(int session_id, char *destination_ip);
void init_tls_handler();