#include <stdio.h>
#include <string.h>

// RC4 key-scheduling algorithm (KSA)
void rc4_initialize(unsigned char *S, unsigned char *key, int key_length)
{
    printf("\nMatrix BEFORE the swap (Initial State of S): \n");

    int i, j = 0, temp;

    // Initialize the state vector S
    for (i = 0; i < 256; i++)
    {
        S[i] = i;
        printf("%d ", S[i]); // Print the state vector S[i]

        if (i % 16 == 15)
        { // Print new line after every 16 elements
            printf("\n");
        }
    }
    printf("\n");

    // Key-scheduling algorithm (KSA)
    printf("\nMatrix AFTER the swap (State of S after KSA): \n");

    for (i = 0; i < 256; i++)
    {
        j = (j + S[i] + key[i % key_length]) % 256;
        temp = S[i];
        S[i] = S[j];
        S[j] = temp;

        // Print the updated state vector S[i] after the swap
        printf("%d ", S[i]);

        if (i % 16 == 15)
        { // Print new line after every 16 elements
            printf("\n");
        }
    }
    printf("\n");
}

// RC4 pseudorandom generation algorithm (PRGA)
void rc4_crypt(unsigned char *S, unsigned char *data, int data_length)
{
    int i = 0, j = 0, k, temp;
    for (k = 0; k < data_length; k++)
    {
        i = (i + 1) % 256;
        j = (j + S[i]) % 256;
        temp = S[i];
        S[i] = S[j];
        S[j] = temp;

        unsigned char T = S[(S[i] + S[j]) % 256];
        data[k] ^= T;
    }
}

// Function to encrypt or decrypt the data
void rc4(unsigned char *key, int key_length, unsigned char *data, int data_length)
{
    unsigned char S[256];
    rc4_initialize(S, key, key_length);
    rc4_crypt(S, data, data_length);
}

int main()
{
    // Define a key and data to encrypt/decrypt
    unsigned char key[] = "Key";                     // Key for RC4 (can be any string or byte array)
    unsigned char data[] = "Hello, RC4 Encryption!"; // Data to be encrypted

    int key_length = strlen((char *)key);
    int data_length = strlen((char *)data);

    // Encrypt the data
    printf("Original data: %s\n", data);
    rc4(key, key_length, data, data_length);
    printf("Encrypted data: ");
    for (int i = 0; i < data_length; i++)
    {
        printf("%02X ", data[i]);
    }
    printf("\n");

    // Decrypt the data (RC4 is symmetric, so we use the same function)
    rc4(key, key_length, data, data_length);
    printf("Decrypted data: %s\n", data);

    return 0;
}
