#include <stdlib.h>

void string_to_bytes(const char *str, unsigned char *byte_array);
unsigned long long bytes_to_number(const unsigned char *byte_array, size_t length);
void number_to_bytes(unsigned long long number, unsigned char *byte_array, size_t length);
void bytes_to_string(const unsigned char *byte_array, size_t length, char *str);