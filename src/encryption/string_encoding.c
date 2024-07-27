#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Convert a string to a byte array (ASCII encoding)
void string_to_bytes(const char *str, unsigned char *byte_array) {
    size_t length = strlen(str);
    for (size_t i = 0; i < length; i++) {
        byte_array[i] = (unsigned char)str[i];
    }
}

// Convert byte array to a big-endian number
unsigned long long bytes_to_number(const unsigned char *byte_array, size_t length) {
    unsigned long long number = 0;
    for (size_t i = 0; i < length; i++) {
        number = (number << 8) | byte_array[i];
    }
    return number;
}

// Convert a number back to a byte array
void number_to_bytes(unsigned long long number, unsigned char *byte_array, size_t length) {
    for (size_t i = 0; i < length; i++) {
        byte_array[length - 1 - i] = (unsigned char)(number & 0xFF);
        number >>= 8;
    }
}

// Convert byte array back to a string
void bytes_to_string(const unsigned char *byte_array, size_t length, char *str) {
    for (size_t i = 0; i < length; i++) {
        str[i] = (char)byte_array[i];
    }
    str[length] = '\0'; // Null-terminate the string
}
