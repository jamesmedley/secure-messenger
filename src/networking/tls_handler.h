#include "message_decoding.h"
#include "message_encoding.h"
#include "networking.h"

void set_gui_update_callback(void (*callback)(const char *name, const char *message));
void send_message(const char *msg_content, int content_length, char *destination_ip);
void send_client_hello(char *destination_ip);
void init_tls_handler();