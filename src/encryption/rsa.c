#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include "rsa.h"

// Check if number is prime
bool is_prime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;
    if (n % 2 == 0 || n % 3 == 0) return false;
    
    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }
    return true;
}

// Function to generate a random prime number within a given range
int generate_random_prime(int min, int max) {
    int prime;
    bool found = false;

    while (!found) {
        int num = min + rand() % (max - min + 1);
        if (is_prime(num)) {
            prime = num;
            found = true;
        }
    }
    return prime;
}

// Function to return gcd of a and b
int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

// A simple method to evaluate Euler Totient Function
int eulers_totient(unsigned int p, unsigned int q)
{
    return (p-1)*(q-1);
}

// Function to compute modular inverse of a with respect to m
int mod_inverse(int a, int m) {
    int m0 = m, t, q;
    int x0 = 0, x1 = 1;
    
    if (m == 1)
        return 0;
    
    while (a > 1) {
        // q is quotient
        q = a / m;
        t = m;
        
        // m is remainder now, process same as Euclid's algo
        m = a % m, a = t;
        t = x0;
        
        // Update x0 and x1
        x0 = x1 - q * x0;
        x1 = t;
    }
    
    // Make x1 positive
    if (x1 < 0)
        x1 += m0;
    
    return x1;
}


int generate_public_exponent(int phi_n) {
    int e;
    bool found = false;

    while (!found) {
        // Generate a random number for e
        e = 2 + rand() % (phi_n - 2); // Ensure e is between 2 and phi_n - 1
        
        // Check if gcd(e, phi_n) is 1
        if (gcd(e, phi_n) == 1) {
            found = true;
        }
    }

    return e;
}

// Encrypt a message using RSA
unsigned long long encrypt(unsigned long long message, RSA_Key key) {
    unsigned long long result = 1;
    unsigned long long base = message % key.n;
    unsigned long long exp = key.e;
    
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % key.n;
        }
        base = (base * base) % key.n;
        exp /= 2;
    }
    
    return result;
}

// Decrypt a message using RSA
unsigned long long decrypt(unsigned long long ciphertext, RSA_Key key) {
    unsigned long long result = 1;
    unsigned long long base = ciphertext % key.n;
    unsigned long long exp = key.d;
    
    while (exp > 0) {
        if (exp % 2 == 1) {
            result = (result * base) % key.n;
        }
        base = (base * base) % key.n;
        exp /= 2;
    }
    
    return result;
}


RSA_Key generate_keys(){
    RSA_Key key;
    int min = 10000;
    int max = 100000;
    srand(time(0));

    int p = generate_random_prime(min, max);
    int q = generate_random_prime(min, max);
    printf("p: %d, q: %d\n",p,q);

    int n = p * q;
    printf("n: %d\n",n);

    int phi_n = eulers_totient(p, q);

    int e = generate_public_exponent(phi_n);
    printf("e: %d\n",e);

    int d = mod_inverse(e, phi_n);
    printf("d: %d\n", d);

    key.n = n;
    key.e = e;
    key.d = d;

    return key;
}


int main() {
    RSA_Key key = generate_keys();
    
    unsigned long long message = 111; // Example message
    unsigned long long ciphertext = encrypt(message, key);
    unsigned long long decrypted_message = decrypt(ciphertext, key);

    printf("Original message: %llu\n", message);
    printf("Encrypted message: %llu\n", ciphertext);
    printf("Decrypted message: %llu\n", decrypted_message);

    return 0;
}