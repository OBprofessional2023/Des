#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define BLOCK_SIZE 8
#define KEY_SIZE 8
#define KEY56_SIZE 7     // 56 bits (after PC1)
#define KEY48_SIZE 6     // 48 bits (after PC2)
#define HALF_KEY_SIZE 4  // 28 bits for each half
#define HALF_KEY_SIZE2 3 // 28 bits for each half

// Example tables for IP, FP, S-Boxes, etc. (these need to be populated with the actual values)
// int IP[64] = {/* Define initial permutation table */};
int IP[] = {58, 50, 42, 34, 26, 18, 10, 2,
            60, 52, 44, 36, 28, 20, 12, 4,
            62, 54, 46, 38, 30, 22, 14, 6,
            64, 56, 48, 40, 32, 24, 16, 8,
            57, 49, 41, 33, 25, 17, 9, 1,
            59, 51, 43, 35, 27, 19, 11, 3,
            61, 53, 45, 37, 29, 21, 13, 5,
            63, 55, 47, 39, 31, 23, 15, 7};
// int FP[64] = {/* Define final permutation table */};
int FP[] = {40, 8, 48, 16, 56, 24, 64, 32,
            39, 7, 47, 15, 55, 23, 63, 31,
            38, 6, 46, 14, 54, 22, 62, 30,
            37, 5, 45, 13, 53, 21, 61, 29,
            36, 4, 44, 12, 52, 20, 60, 28,
            35, 3, 43, 11, 51, 19, 59, 27,
            34, 2, 42, 10, 50, 18, 58, 26,
            33, 1, 41, 9, 49, 17, 57, 25};
// int E[48] = {/* Define expansion table */};
int E[] = {32, 1, 2, 3, 4, 5,
           4, 5, 6, 7, 8, 9,
           8, 9, 10, 11, 12, 13,
           12, 13, 14, 15, 16, 17,
           16, 17, 18, 19, 20, 21,
           20, 21, 22, 23, 24, 25,
           24, 25, 26, 27, 28, 29,
           28, 29, 30, 31, 32, 1};

int P[] = {16, 7, 20, 21, 29, 12, 28, 17,
           1, 15, 23, 26, 5, 18, 31, 10,
           2, 8, 24, 14, 32, 27, 3, 9,
           19, 13, 30, 6, 22, 11, 4, 25};

// You need the S-Boxes (8 of them, each 6x4 matrix) for the substitution step
// int S[8][64] = {/* Define the 8 S-boxes */};
int S[8][64] = {
    {14, 4, 13, 1, 2, 15, 11, 8, 3, 10, 6, 12, 5, 9, 0, 7,
     0, 15, 7, 4, 14, 2, 13, 1, 10, 6, 12, 11, 9, 5, 3, 8,
     4, 1, 14, 8, 13, 6, 2, 11, 15, 12, 9, 7, 3, 10, 5, 0,
     15, 12, 8, 2, 4, 9, 1, 7, 5, 11, 3, 14, 10, 0, 6, 13}, // S1

    {15, 1, 8, 14, 6, 11, 3, 4, 9, 7, 2, 13, 12, 0, 5, 10,
     3, 13, 4, 7, 15, 2, 8, 14, 12, 0, 1, 10, 6, 9, 11, 5,
     0, 14, 7, 11, 10, 4, 13, 1, 5, 8, 12, 6, 9, 3, 2, 15,
     13, 8, 10, 1, 3, 15, 4, 2, 11, 6, 7, 12, 0, 5, 14, 9}, // S2

    {10, 0, 9, 14, 6, 3, 15, 5, 1, 13, 12, 7, 11, 4, 2, 8,
     13, 7, 0, 9, 3, 4, 6, 10, 2, 8, 5, 14, 12, 11, 15, 1,
     13, 6, 4, 9, 8, 15, 3, 0, 11, 1, 2, 12, 5, 10, 14, 7,
     1, 10, 13, 0, 6, 9, 8, 7, 4, 15, 14, 3, 11, 5, 2, 12}, // S3

    {7, 13, 14, 3, 0, 6, 9, 10, 1, 2, 8, 5, 11, 12, 4, 15,
     13, 8, 11, 5, 6, 15, 0, 3, 4, 7, 2, 12, 1, 10, 14, 9,
     10, 6, 9, 0, 12, 11, 7, 13, 15, 1, 3, 14, 5, 2, 8, 4,
     3, 15, 0, 6, 10, 1, 13, 8, 9, 4, 5, 11, 12, 7, 2, 14}, // S4

    {2, 12, 4, 1, 7, 10, 11, 6, 8, 5, 3, 15, 13, 0, 14, 9,
     14, 11, 2, 12, 4, 7, 13, 1, 5, 0, 15, 10, 3, 9, 8, 6,
     4, 2, 1, 11, 10, 13, 7, 8, 15, 9, 12, 5, 6, 3, 0, 14,
     11, 8, 12, 7, 1, 14, 2, 13, 6, 15, 0, 9, 10, 4, 5, 3}, // S5

    {12, 1, 10, 15, 9, 2, 6, 8, 0, 13, 3, 4, 14, 7, 5, 11,
     10, 15, 4, 2, 7, 12, 9, 5, 6, 1, 13, 14, 0, 11, 3, 8,
     9, 14, 15, 5, 2, 8, 12, 3, 7, 0, 4, 10, 1, 13, 11, 6,
     4, 3, 2, 12, 9, 5, 15, 10, 11, 14, 1, 7, 6, 0, 8, 13}, // S6

    {4, 11, 2, 14, 15, 0, 8, 13, 3, 12, 9, 7, 5, 10, 6, 1,
     13, 0, 11, 7, 4, 9, 1, 10, 14, 3, 5, 12, 2, 15, 8, 6,
     1, 4, 11, 13, 12, 3, 7, 14, 10, 15, 6, 8, 0, 5, 9, 2,
     6, 11, 13, 8, 1, 4, 10, 7, 9, 5, 0, 15, 14, 2, 3, 12}, // S7

    {13, 2, 8, 4, 6, 15, 11, 1, 10, 9, 3, 14, 5, 0, 12, 7,
     1, 15, 13, 8, 10, 3, 7, 4, 12, 5, 6, 11, 0, 14, 9, 2,
     7, 11, 4, 1, 9, 12, 14, 2, 0, 6, 10, 13, 15, 3, 5, 8,
     2, 1, 14, 7, 4, 10, 8, 13, 15, 12, 9, 0, 3, 5, 6, 11} // S8
};

// 3. Generate 16 round keys
unsigned char round_keys[16][6];
// int round_keysD[16][48];

// Key rotation schedule
const int key_shifts[16] = {1, 1, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1, 2, 1};

// Key generation constants
// int PC1[56] = {/* Permutation Choice 1 */};
int PC1[] = {57, 49, 41, 33, 25, 17, 9,
             1, 58, 50, 42, 34, 26, 18,
             10, 2, 59, 51, 43, 35, 27,
             19, 11, 3, 60, 52, 44, 36,
             63, 55, 47, 39, 31, 23, 15,
             7, 62, 54, 46, 38, 30, 22,
             14, 6, 61, 53, 45, 37, 29,
             21, 13, 5, 28, 20, 12, 4};
// int PC2[48] = {/* Permutation Choice 2 */};
int PC2[] = {14, 17, 11, 24, 1, 5,
             3, 28, 15, 6, 21, 10,
             23, 19, 12, 4, 26, 8,
             16, 7, 27, 20, 13, 2,
             41, 52, 31, 37, 47, 55,
             30, 40, 51, 45, 33, 48,
             44, 49, 39, 56, 34, 53,
             46, 42, 50, 36, 29, 32};

void print_bits(unsigned char *arr, int size)
{
    for (int i = 0; i < size; i++)
    {
        for (int j = 7; j >= 0; j--)
        {
            printf("%d", (arr[i] >> j) & 1);
        }
        printf(" ");
    }
    printf("\n");
}

// Left shift function
void left_shift(unsigned char array[4], int shift_count)
{
    for (int i = 0; i < shift_count; i++)
    {
        unsigned char carry = (array[0] >> 7) & 0x01; // Store the leftmost bit

        for (int j = 0; j < 3; j++)
        { // Iterate up to the second to last element
            array[j] = (array[j] << 1) | ((array[j + 1] >> 7) & 0x01);
        }

        // Handle the last element, wrapping the carry
        array[3] = (array[3] << 1) | carry;
    }
}

// Perform the PC1 permutation on the 64-bit key
void permute_PC1(unsigned char key64[8], unsigned char key56[7])
{
    // Loop through each of the 56 positions in PC-1
    for (int i = 0; i < 56; i++)
    {
        int pc1_index = PC1[i] - 1;     // Convert to 0-based index
        int byte_index = pc1_index / 8; // Determine which byte the bit is in
        int bit_index = pc1_index % 8;  // Determine which bit within the byte

        // Extract the bit from the original key (key64)
        // Corrected byte_index access: should be within the bounds of key64 (0 to 7)
        unsigned char bit = (key64[byte_index] >> (7 - bit_index)) & 1;

        // Set the corresponding bit in key56
        int key56_byte_index = i / 8; // Determine byte position in key56
        int key56_bit_index = i % 8;  // Determine bit position in key56

        key56[key56_byte_index] |= (bit << (7 - key56_bit_index)); // Set the bit in key56
    }
    // Removed the premature return statement. The loop needs to complete.
}

// Function to perform the expansion permutation (E)
void permute_E(const unsigned char key64[4], unsigned char key56[6])
{
    // Initialize the 56-bit key (unsigned char array of size 7)
    // unsigned long long result = 0;

    // Loop through each of the 56 positions in PC-1
    for (int i = 0; i < 48; i++)
    {
        int pc1_index = E[i] - 1;       // Convert to 0-based index
        int byte_index = pc1_index / 8; // Determine which byte the bit is in
        int bit_index = pc1_index % 8;  // Determine which bit within the byte

        // Extract the bit from the original key (key64)
        unsigned char bit = (key64[byte_index] >> (7 - bit_index)) & 1;

        // Set the corresponding bit in key56
        int key56_byte_index = i / 8; // Determine byte position in key56
        int key56_bit_index = i % 8;  // Determine bit position in key56

        key56[key56_byte_index] |= (bit << (7 - key56_bit_index)); // Set the bit in key56
    }
}

// Perform the P permutation on the key
void permute_P(const unsigned char key64[4], unsigned char key56[4])
{
    // Initialize the 56-bit key (unsigned char array of size 7)
    // unsigned long long result = 0;

    // Loop through each of the 56 positions in PC-1
    for (int i = 0; i < 32; i++)
    {
        int pc1_index = P[i] - 1;       // Convert to 0-based index
        int byte_index = pc1_index / 8; // Determine which byte the bit is in
        int bit_index = pc1_index % 8;  // Determine which bit within the byte

        // Extract the bit from the original key (key64)
        unsigned char bit = (key64[byte_index] >> (7 - bit_index)) & 1;

        // Set the corresponding bit in key56
        int key56_byte_index = i / 8; // Determine byte position in key56
        int key56_bit_index = i % 8;  // Determine bit position in key56

        key56[key56_byte_index] |= (bit << (7 - key56_bit_index)); // Set the bit in key56
    }
}

// Perform the IP permutation on the 64-bit key
void permute_IP(unsigned char key64[8], unsigned char key56[8])
{
    // Loop through each of the 56 positions in PC-1
    for (int i = 0; i < 64; i++)
    {
        int pc1_index = IP[i] - 1;      // Convert to 0-based index
        int byte_index = pc1_index / 8; // Determine which byte the bit is in
        int bit_index = pc1_index % 8;  // Determine which bit within the byte

        // Extract the bit from the original key (key64)
        // Corrected byte_index access: should be within the bounds of key64 (0 to 7)
        unsigned char bit = (key64[byte_index] >> (7 - bit_index)) & 1;

        // Set the corresponding bit in key56
        int key56_byte_index = i / 8; // Determine byte position in key56
        int key56_bit_index = i % 8;  // Determine bit position in key56

        key56[key56_byte_index] |= (bit << (7 - key56_bit_index)); // Set the bit in key56
    }
    // Removed the premature return statement. The loop needs to complete.
}

// Perform the PC1 permutation on the 64-bit key
void permute_FP(unsigned char key64[8], unsigned char key56[8])
{
    // Loop through each of the 56 positions in PC-1
    for (int i = 0; i < 64; i++)
    {
        int pc1_index = FP[i] - 1;      // Convert to 0-based index
        int byte_index = pc1_index / 8; // Determine which byte the bit is in
        int bit_index = pc1_index % 8;  // Determine which bit within the byte

        // Extract the bit from the original key (key64)
        // Corrected byte_index access: should be within the bounds of key64 (0 to 7)
        unsigned char bit = (key64[byte_index] >> (7 - bit_index)) & 1;

        // Set the corresponding bit in key56
        int key56_byte_index = i / 8; // Determine byte position in key56
        int key56_bit_index = i % 8;  // Determine bit position in key56

        key56[key56_byte_index] |= (bit << (7 - key56_bit_index)); // Set the bit in key56
    }
    // Removed the premature return statement. The loop needs to complete.
}

// Perform the PC2 permutation to generate the subkey
void permute_PC2(unsigned char key56[7], unsigned char key48[6])
{
    // Loop through each of the 56 positions in PC-1
    for (int i = 0; i < 48; i++)
    {
        int pc1_index = PC2[i] - 1;     // Convert to 0-based index
        int byte_index = pc1_index / 8; // Determine which byte the bit is in
        int bit_index = pc1_index % 8;  // Determine which bit within the byte

        // Extract the bit from the original key (key64)
        // Corrected byte_index access: should be within the bounds of key64 (0 to 7)
        unsigned char bit = (key56[byte_index] >> (7 - bit_index)) & 1;

        // Set the corresponding bit in key56
        int key48_byte_index = i / 8; // Determine byte position in key56
        int key48_bit_index = i % 8;  // Determine bit position in key56

        key48[key48_byte_index] |= (bit << (7 - key48_bit_index)); // Set the bit in key56
    }
    // Removed the premature return statement. The loop needs to complete.
}

void split_key(unsigned char key56[7], unsigned char left[4], unsigned char right[4])
{
    // Copy the first 28 bits (3.5 bytes) to the left part
    memcpy(left, key56, 3);
    left[3] = (key56[3] & 0xF0) >> 4;
    left[3] <<= 4;

    // Copy the last 28 bits (3.5 bytes) to the right part
    right[0] = (key56[3] & 0x0F) << 4 | (key56[4] & 0xF0) >> 4;
    right[1] = (key56[4] & 0x0F) << 4 | (key56[5] & 0xF0) >> 4;
    right[2] = (key56[5] & 0x0F) << 4 | (key56[6] & 0xF0) >> 4;
    right[3] = (key56[6] & 0x0F) << 4; // Rest is padded with 0 implicitly
}

// Function to generate the 16 round keys from the original key
void generate_keys(unsigned char key[8], unsigned char round_keys[16][6])
{
    unsigned char key56[7] = {0}; // 56-bit key after PC1

    // Step 1: Permute the key using PC1 (56 bits)
    permute_PC1(key, key56); // Now the key is in a 56-bit form

    // Step 2: Split the key into two 28-bit halves
    unsigned char left[4] = {0};  // 28 bits (stored in 4 bytes)
    unsigned char right[4] = {0}; // 28 bits (stored in 4 bytes)

    // Note: The least significant 4 bits of key56[6] are not used in the standard
    split_key(key56, left, right);

    // Step 3: Perform 16 rounds of shifting and applying PC-2
    for (int round = 0; round < 16; round++)
    {
        // Step 3.1: Perform left shifts
        left_shift(left, key_shifts[round]);  // Left shift the left part
        left_shift(right, key_shifts[round]); // Left shift the right part

        // Step 3.2: Combine C and D to get the 56-bit key
        unsigned char key56_combined[7] = {0};

        memcpy(key56_combined, left, 3);
        // memcpy(key56_combined + 4, right + 1, 3);

        key56_combined[3] = (left[3] & 0xF0) | (right[0] & 0xF0) >> 4;
        key56_combined[4] = (right[0] & 0x0F) << 4 | (right[1] & 0xF0) >> 4;
        key56_combined[5] = (right[1] & 0x0F) << 4 | (right[2] & 0xF0) >> 4;
        key56_combined[6] = (right[2] & 0x0F) << 4 | (right[3] & 0xF0) >> 4;
        // = (key56[3] & 0x0F) << 4 | (key56[4] & 0xF0) >> 4;

        // // Apply PC2 to generate the 48-bit subkey
        permute_PC2(key56_combined, round_keys[round]);
    }
}

void f_function(unsigned char right_half[4], unsigned char round_key[6], unsigned char output[4])
{
    // Step 1: Expand the right half using the expansion table
    unsigned char expanded[6] = {0};
    permute_E(right_half, expanded);

    // printf("\n");
    // printf("Expanded in binary: \n");
    // print_bits(expanded, 6); // Print the 64-bit result (8 bytes)

    // Step 2: XOR the expanded data with the round key
    for (int i = 0; i < 6; i++)
    {
        expanded[i] = expanded[i] ^ round_key[i];
    }

    // Step 3: Apply S-boxes (8 S-boxes, each 6 bits to 4 bits)
    unsigned char sbox_output[8] = {0}; // This will store the 4-bit output from each S-box
    for (int i = 0; i < 8; i++)
    {
        // Extract the 6-bit block from the expanded array
        int block = (expanded[i * 6] << 5) | (expanded[i * 6 + 1] << 4) | (expanded[i * 6 + 2] << 3) |
                    (expanded[i * 6 + 3] << 2) | (expanded[i * 6 + 4] << 1) | expanded[i * 6 + 5];

        // Step 3.1: The first and last bits of the 6-bit block represent the row
        int row = (block >> 5) | (block & 1); // Row = first and last bits
        // Step 3.2: The middle 4 bits represent the column
        int col = (block >> 1) & 0x0F; // Column = middle 4 bits

        // Step 3.3: Look up the S-box value (6-bit input -> 4-bit output)
        int sbox_value = S[i][row * 16 + col]; // S-box value is at [row * 16 + col]

        // Step 3.4: Store the 4-bit output of the S-box
        sbox_output[i] = (unsigned char)sbox_value; // Store the 4-bit value as an 8-bit byte
    }

    // unsigned char output3[4] = {0};

    // Step 4: Combine the 4-bit outputs of the S-boxes into a 32-bit value
    unsigned char combined_output[4] = {0}; // Store the 32-bit combined output from all S-boxes

    // Each S-box produces a 4-bit result, so we combine them into a single 32-bit value
    for (int i = 0; i < 8; i++)
    {
        combined_output[i / 2] |= sbox_output[i] << (4 * (1 - (i % 2))); // Combine 2 consecutive S-box outputs into one byte
    }

    // Step 5: Permute the result using the permutation table P
    unsigned char output2[4] = {0};
    permute_P(combined_output, output2);

    for (int i = 0; i < 4; i++)
    {
        output[i] = output2[i];
    }
}

// The main DES encryption function
void desE(unsigned char *input, unsigned char *output)
{
    // Step 1: Apply the initial permutation (IP)
    // unsigned char key56[7] = {0};

    unsigned char permuted_input[8] = {0};

    permute_IP(input, permuted_input); // Apply IP to the input

    // Step 2: Split the 64-bit permuted input into two 32-bit halves (C and D)

    unsigned char left[4], right[4];      // Left and right halves of the key
    memcpy(left, permuted_input, 4);      // C0 = first 28 bits
    memcpy(right, permuted_input + 4, 4); // D0 = last 28 bits

    // Step 3: Perform 16 rounds of DES
    for (int round = 0; round < 16; round++)
    {
        // Apply the F-function to the right half and round key
        unsigned char new_left[4], new_right[4];
        f_function(right, round_keys[round], new_left);

        // XOR the result of the F-function with the left half and update right
        for (int i = 0; i < 4; i++)
        {
            // left[i] = right[i];
            new_right[i] = left[i] ^ new_left[i]; // XOR left and the result of the F-function
        }

        memcpy(left, right, 4);
        memcpy(right, new_right, 4);
    }

    // 4. Combine left and right and apply the final permutation (IP for decryption)
    unsigned char final_data[8] = {0};
    unsigned char final_data_permuted[8] = {0};

    for (int i = 0; i < 4; i++)
    {
        final_data[i] = left[i];
        final_data[i + 4] = right[i]; // XOR left and the result of the F-function
    }

    permute_FP(final_data, final_data_permuted); // Correct final permutation for decryption

    // 5. Output the final decrypted data
    // for (int i = 0; i < 8; i++)
    // {
    //     output[i] = final_data_permuted[i];
    // }
    memcpy(output, final_data_permuted, 4);
}

// The main DES decryption function
void desD(unsigned char *input, unsigned char *output)
{
    // Step 1: Apply the initial permutation (IP)
    // unsigned char key56[7] = {0};

    unsigned char permuted_input[8] = {0};

    permute_IP(input, permuted_input); // Apply IP to the input

    // Step 2: Split the 64-bit permuted input into two 32-bit halves (C and D)

    unsigned char left[4], right[4];      // Left and right halves of the key
    memcpy(left, permuted_input, 4);      // C0 = first 28 bits
    memcpy(right, permuted_input + 4, 4); // D0 = last 28 bits

    // Step 3: Perform 16 rounds of DES
    for (int round = 15; round >= 0; round--)
    {
        // Apply the F-function to the right half and round key
        unsigned char new_left[4], new_right[4];
        f_function(right, round_keys[round], new_left);

        // XOR the result of the F-function with the left half and update right
        for (int i = 0; i < 4; i++)
        {
            // left[i] = right[i];
            new_right[i] = left[i] ^ new_left[i]; // XOR left and the result of the F-function
        }

        memcpy(left, right, 4);
        memcpy(right, new_right, 4);
    }

    // 4. Combine left and right and apply the final permutation (IP for decryption)
    unsigned char final_data[8] = {0};
    unsigned char final_data_permuted[8] = {0};

    for (int i = 0; i < 4; i++)
    {
        final_data[i] = left[i];
        final_data[i + 4] = right[i]; // XOR left and the result of the F-function
    }

    permute_FP(final_data, final_data_permuted); // Correct final permutation for decryption

    // 5. Output the final decrypted data
    // for (int i = 0; i < 8; i++)
    // {
    //     output[i] = final_data_permuted[i];
    // }
    memcpy(output, final_data_permuted, 4);
}

int main()
{
    // Example data (64 bits) and key (56 bits)
    // 64-bit input block (message data) as an array of integers (8 bytes)
    unsigned char input[8] = {116, 104, 105, 115, 107, 101, 121, 10};

    // // 56-bit key as an array of integers (8 bytes)
    unsigned char key[8] = {121, 111, 119, 104, 97, 116, 117, 112};
    // Example 64-bit key for DES
    // This is your original example key

    unsigned char output[8];
    unsigned char output2[8];

    printf("\n");
    printf("Input in binary: \n");
    print_bits(input, 8); // Print the 64-bit result (8 bytes)

    printf("\n");
    printf("Input in binary: \n");
    print_bits(input, 8); // Print the 64-bit result (8 bytes)

    printf("\n");
    printf("Key in binary: \n");
    print_bits(key, 8); // Print the 64-bit result (8 bytes)

    // Use generate_keys for encryption
    generate_keys(key, round_keys); // Generate the round keys for encryption
    printf("\n");

    for (int i = 0; i < 16; i++)
    {
        printf("Round %d Key: ", i + 1);
        printf("\n");
        // printf("Key in binary: \n");
        print_bits(round_keys[i], 6); // Print the 64-bit result (8 bytes)
    }

    // // Encrypt the data

    desE(input, output);

    printf("\n");
    printf("Output in binary: \n");
    print_bits(output, 8); // Print the 64-bit result (8 bytes)

    // // printf("\n");
    // // printf("\n");

    // printf("This is the output: \n");
    // for (int i = 0; i < 8; i++)
    // {
    //     printf("%d ", output[i]);
    // }

    // desD(output, output2);

    // printf("\n");
    // printf("Output2 in binary: \n");
    // print_bits(output2, 8); // Print the 64-bit result (8 bytes)

    // // printf("\n");
    // // printf("\n");

    // printf("This is the output2: \n");
    // for (int i = 0; i < 8; i++)
    // {
    //     printf("%d ", output2[i]);
    // }

    return 0;
}
