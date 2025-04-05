#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define BLOCK_SIZE 8
#define KEY_SIZE 8

// Example tables for IP, FP, S-Boxes, etc. (these need to be populated with the actual values)
// int IP[64] = {/* Define initial permutation table */};
uint64_t IP[] = {58, 50, 42, 34, 26, 18, 10, 2,
                 60, 52, 44, 36, 28, 20, 12, 4,
                 62, 54, 46, 38, 30, 22, 14, 6,
                 64, 56, 48, 40, 32, 24, 16, 8,
                 57, 49, 41, 33, 25, 17, 9, 1,
                 59, 51, 43, 35, 27, 19, 11, 3,
                 61, 53, 45, 37, 29, 21, 13, 5,
                 63, 55, 47, 39, 31, 23, 15, 7};
// int FP[64] = {/* Define final permutation table */};
uint64_t FP[] = {40, 8, 48, 16, 56, 24, 64, 32,
                 39, 7, 47, 15, 55, 23, 63, 31,
                 38, 6, 46, 14, 54, 22, 62, 30,
                 37, 5, 45, 13, 53, 21, 61, 29,
                 36, 4, 44, 12, 52, 20, 60, 28,
                 35, 3, 43, 11, 51, 19, 59, 27,
                 34, 2, 42, 10, 50, 18, 58, 26,
                 33, 1, 41, 9, 49, 17, 57, 25};
// int E[48] = {/* Define expansion table */};
uint64_t E[] = {32, 1, 2, 3, 4, 5,
                4, 5, 6, 7, 8, 9,
                8, 9, 10, 11, 12, 13,
                12, 13, 14, 15, 16, 17,
                16, 17, 18, 19, 20, 21,
                20, 21, 22, 23, 24, 25,
                24, 25, 26, 27, 28, 29,
                28, 29, 30, 31, 32, 1};

uint64_t P[] = {16, 7, 20, 21,
                29, 12, 28, 17,
                1, 15, 23, 26,
                5, 18, 31, 10,
                2, 8, 24, 14,
                32, 27, 3, 9,
                19, 13, 30, 6,
                22, 11, 4, 25};

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
uint64_t PC1[] = {57, 49, 41, 33, 25, 17, 9,
                  1, 58, 50, 42, 34, 26, 18,
                  10, 2, 59, 51, 43, 35, 27,
                  19, 11, 3, 60, 52, 44, 36,
                  63, 55, 47, 39, 31, 23, 15,
                  7, 62, 54, 46, 38, 30, 22,
                  14, 6, 61, 53, 45, 37, 29,
                  21, 13, 5, 28, 20, 12, 4};
// int PC2[48] = {/* Permutation Choice 2 */};
uint64_t PC2[] = {14, 17, 11, 24, 1, 5,
                  3, 28, 15, 6, 21, 10,
                  23, 19, 12, 4, 26, 8,
                  16, 7, 27, 20, 13, 2,
                  41, 52, 31, 37, 47, 55,
                  30, 40, 51, 45, 33, 48,
                  44, 49, 39, 56, 34, 53,
                  46, 42, 50, 36, 29, 32};

// Function to permute the key based on a given permutation table (PC1 or PC2)
void permute(uint64_t *input, const uint64_t *table, int table_size)
{
    uint64_t output = 0;
    for (int i = 0; i < table_size; i++)
    {
        int bit = (*input >> (63 - table[i])) & 1;
        output |= (bit << (table_size - 1 - i));
    }
    *input = output;
}

void permute2(uint32_t *input, const uint64_t *table, int table_size)
{
    uint64_t output = 0;
    for (int i = 0; i < table_size; i++)
    {
        int bit = (*input >> (63 - table[i])) & 1;
        output |= (bit << (table_size - 1 - i));
    }
    *input = output;
}

// Function to shift left by one bit
void leftShift(uint32_t *half, int shift_amount)
{
    *half = (*half << shift_amount) | (*half >> (28 - shift_amount));
    *half &= 0xFFFFFFF; // Keep only 28 bits
}

// Function to generate the 16 round keys from the original key
void generate_keys(uint64_t *key, unsigned char round_keys[16][6])
{
    // Step 1: Permute the key using PC1 (56 bits)
    permute(key, PC1, 56); // Now the key is in a 56-bit form (though it is still stored in a 64-bit variable)

    // Step 2: Split the key into two 28-bit halves
    uint32_t left = (uint32_t)((*key >> 28) & 0xFFFFFFF); // First 28 bits (C)
    uint32_t right = (uint32_t)(*key & 0xFFFFFFF);        // Last 28 bits (D)

    // Step 3: Perform 16 rounds of shifting and applying PC-2
    for (int round = 0; round < 16; round++)
    {
        // Step 3.1: Perform left shifts
        leftShift(&left, key_shifts[round]);
        leftShift(&right, key_shifts[round]);

        // Step 3.2: Combine the two halves back into a 56-bit key
        uint64_t combined = ((uint64_t)left << 28) | right;

        // Step 3.3: Apply PC2 to get a 48-bit round key
        permute(&combined, PC2, 48); // Apply PC2 (output is a 48-bit key)

        // Step 3.4: Store the round key (6 bytes, 48 bits)
        for (int i = 0; i < 6; i++)
        {
            round_keys[round][i] = (unsigned char)((combined >> (40 - i * 8)) & 0xFF);
        }
    }
}

void f_function(uint32_t *right_half, unsigned char *round_key, uint64_t *output)
{
    // Step 1: Expand the right half using the expansion table
    int expanded[6];
    permute2(right_half, E, 48);

    // Step 2: XOR the expanded data with the round key
    for (int i = 0; i < 6; i++)
    {
        expanded[i] ^= round_key[i];
    }

    // Step 3: Apply S-boxes (8 S-boxes, each 6 bits to 4 bits)
    int sbox_output[8][4]; // This will store the 4-bit output from each S-box
    for (int i = 0; i < 8; i++)
    {
        // Extract the 6-bit block from the expanded array
        int block[6];
        for (int j = 0; j < 6; j++)
        {
            block[j] = expanded[i * 6 + j];
        }

        // Step 3.1: The first and last bits of the 6-bit block represent the row
        int row = (block[0] << 1) | block[5]; // Row = first bit + last bit
        // Step 3.2: The middle 4 bits represent the column
        int col = (block[1] << 3) | (block[2] << 2) | (block[3] << 1) | block[4]; // Column = middle 4 bits

        // Step 3.3: Look up the S-box value (6-bit input -> 4-bit output)
        int sbox_value = S[i][row * 16 + col]; // S-box value is at [row * 16 + col]

        // Step 3.4: Store the 4-bit output of the S-box
        for (int j = 0; j < 4; j++)
        {
            sbox_output[i][j] = (sbox_value >> (3 - j)) & 1; // Extract each bit of the 4-bit output
        }
    }

    // Step 4: Combine the 4-bit outputs of the S-boxes into a 32-bit value
    for (int i = 0; i < 8; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            output[i * 4 + j] = sbox_output[i][j];
        }
    }

    // Step 5: Permute the result using the permutation table P
    permute(output, P, 32);
}

// The main DES encryption function
void desE(uint64_t *input, uint64_t *output, unsigned char round_keys[16][6])
{
    // Step 1: Apply the initial permutation (IP)
    uint64_t permuted_input = *input;
    permute(&permuted_input, IP, 64); // Apply IP to the input

    // Step 2: Split the 64-bit permuted input into two 32-bit halves (C and D)
    uint32_t left = (uint32_t)(permuted_input >> 32);         // First 32 bits (left half)
    uint32_t right = (uint32_t)(permuted_input & 0xFFFFFFFF); // Last 32 bits (right half)

    // Step 3: Perform 16 rounds of DES
    for (int round = 0; round < 16; round++)
    {
        // Apply the F-function to the right half and round key
        uint64_t new_left = 0, new_right = 0;
        f_function(&right, round_keys[round], &new_left);

        // XOR the result of the F-function with the left half
        new_right = left ^ (uint32_t)new_left; // XOR left and the result of the F-function

        // Update the halves for the next round
        left = right;      // Move right to left for the next round
        right = new_right; // Update right with the XOR result
    }

    // Step 4: Combine the final left and right halves and apply the final permutation (FP)
    uint64_t final_data = ((uint64_t)left << 32) | right; // Combine left and right halves into 64 bits
    permute(&final_data, FP, 64);                         // Apply final permutation (FP)

    // Step 5: Output the final encrypted data
    *output = final_data; // Store the result in the output variable
}

// void desD(int *input, int *output, int round_keys[16][48])
// {
//     // 1. Apply the initial permutation
//     permute(input, IP, 64);

//     // 2. Split the 64-bit input into two 32-bit halves
//     int left[32], right[32];
//     for (int i = 0; i < 32; i++)
//     {
//         left[i] = input[i];
//         right[i] = input[i + 32];
//     }

//     // 3. Perform 16 rounds of DES (but apply round keys in reverse order for decryption)
//     for (int round = 15; round >= 0; round--)
//     {
//         // Apply the F-function and XOR with the left half
//         int new_left[32], new_right[32];
//         f_function(right, round_keys[round], new_left);

//         // Update the halves for the next round
//         for (int i = 0; i < 32; i++)
//         {
//             new_right[i] = left[i] ^ new_left[i];
//         }
//         for (int i = 0; i < 32; i++)
//         {
//             left[i] = right[i];
//             right[i] = new_right[i];
//         }
//     }

//     // 4. Combine left and right and apply the final permutation (IP for decryption)
//     int final_data[64];
//     for (int i = 0; i < 32; i++)
//     {
//         final_data[i] = left[i];
//         final_data[i + 32] = right[i];
//     }
//     permute(final_data, FP, 64); // Correct final permutation for decryption

//     // 5. Output the final decrypted data
//     for (int i = 0; i < 64; i++)
//     {
//         output[i] = final_data[i];
//     }
// }

// Encrypt a single 64-bit block (8 bytes)
// void encrypt_block(int *input, int *round_keys, int *output)
// {
//     // Apply initial permutation (IP)
//     // 1. Apply the initial permutation
//     permute(input, IP, 64);

//     // 2. Split the 64-bit input into two 32-bit halves
//     int L[32], R[32];
//     for (int i = 0; i < 32; i++)
//     {
//         L[i] = input[i];
//         R[i] = input[i + 32];
//     }

//     // Perform 16 rounds of encryption
//     for (int round = 0; round < 16; round++)
//     {
//         // Save L and R
//         int L_new[32], R_new[32];
//         f_function(R, round_keys[round], L_new);
//         for (int i = 0; i < 32; i++)
//         {
//             L_new[i] = R[i]; // L becomes R
//         }

//         // Apply the round function
//         // Round function uses round_keys[round] and modifies R
//         for (int i = 0; i < 32; i++)
//         {
//             R_new[i] = L[i] ^ L_new[i]; // Simplified
//         }

//         // Update L and R
//         for (int i = 0; i < 32; i++)
//         {
//             L[i] = L_new[i];
//             R[i] = R_new[i];
//         }
//     }

//     // Combine L and R back together
//     int combined[64];
//     for (int i = 0; i < 32; i++)
//     {
//         combined[i] = L[i];
//         combined[i + 32] = R[i];
//     }

//     // Apply the final permutation (FP)
//     permute(combined, FP, 64);

//     // Copy result to output
//     for (int i = 0; i < 64; i++)
//     {
//         output[i] = combined[i];
//     }
// }

// void encrypt_file(FILE *input_file, FILE *output_file, int *key)
// {
//     // Generate round keys for DES
//     // int round_keys[16][48];
//     generate_keys(key, round_keys);

//     // for (int i = 0; i < 16; i++)
//     // {
//     //     printf("Round %d Key: ", i + 1);

//     //     // Print each key in binary (48 bits per round)
//     //     for (int j = 0; j < 48; j++)
//     //     {
//     //         printf("%d", round_keys[i][j]);
//     //     }

//     //     printf("\n");
//     // }

//     // Buffer to read input file in chunks of 8 bytes (64 bits)
//     unsigned char input_buffer[8];
//     unsigned char output_buffer[8];
//     size_t bytes_read;

//     // Read the input file in 8-byte (64-bit) chunks
//     while ((bytes_read = fread(input_buffer, 1, 8, input_file)) == 8)
//     {
//         // Convert input_buffer into an integer array (64-bit block)
//         int input_block[2] = {0}; // 2 integers to hold 64 bits (32-bit each)
//         for (int i = 0; i < 8; i++)
//         {
//             input_block[i / 4] |= (input_buffer[i] << (24 - 8 * (i % 4)));
//         }

//         // Encrypt the block
//         encrypt_block(input_block, round_keys, (int *)output_buffer);

//         // Write the encrypted block to the output file
//         fwrite(output_buffer, 1, 8, output_file);
//     }

//     // Handle any remaining bytes (if the file size is not a multiple of 8 bytes)
//     if (bytes_read > 0 && bytes_read < 8)
//     {
//         // You may need padding here (e.g., PKCS7 padding) before encryption
//         memset(input_buffer + bytes_read, 0, 8 - bytes_read); // Pad with zeros
//         int input_block[2] = {0};                             // 2 integers to hold 64 bits (32-bit each)
//         for (int i = 0; i < 8; i++)
//         {
//             input_block[i / 4] |= (input_buffer[i] << (24 - 8 * (i % 4)));
//         }

//         // Encrypt the block
//         encrypt_block(input_block, round_keys, (int *)output_buffer);

//         // Write the padded encrypted block to the output file
//         fwrite(output_buffer, 1, 8, output_file);
//     }
// }

int main()
{
    // Example data (64 bits) and key (56 bits)
    // 64-bit input block (message data) as an array of integers (8 bytes)
    uint64_t input = 0x9474B8E8C73BCA7D;

    // // 56-bit key as an array of integers (8 bytes)
    uint64_t key = 0xA1B2C3D4E5F60708; // Example 64-bit key for DES
                                       // This is your original example key

    // unsigned char key2 = key;

    // int input[64] = {62, 54, 46, 38, 30, 22, 14, 6,
    //                  58, 50, 42, 34, 26, 18, 10, 2,
    //                  60, 52, 44, 36, 28, 20, 12, 4,
    //                  64, 56, 48, 40, 32, 24, 16, 8,
    //                  63, 55, 47, 39, 31, 23, 15, 7,
    //                  59, 51, 43, 35, 27, 19, 11, 3,
    //                  61, 53, 45, 37, 29, 21, 13, 5,
    //                  57, 49, 41, 33, 25, 17, 9, 1};

    // Check the size of the array
    printf("Total size of array in bytes: %zu\n", sizeof(input));
    printf("Size of one element (int): %zu\n", sizeof(input));
    printf("Number of elements: %zu\n", sizeof(input) / sizeof(input));
    printf("\n");
    printf("\n");
    printf("Total size of array in bytes: %zu\n", sizeof(key));
    printf("Size of one element (int): %zu\n", sizeof(key));
    printf("Number of elements: %zu\n", sizeof(key) / sizeof(key));

    // Printing the 64-bit value as hexadecimal
    printf("The value of input in hexadecimal: 0x%llx\n", input);

    // Alternatively, printing the 64-bit value in decimal
    printf("The value of input in decimal: %llu\n", input);

    // Printing the 64-bit value as hexadecimal
    printf("The value of input in hexadecimal: 0x%llx\n", key);

    // Alternatively, printing the 64-bit value in decimal
    printf("The value of input in decimal: %llu\n", key);
    // int key[56] = {1, 2, 3, 4, 5, 6, 7};
    // int key[56] = {57, 49, 41, 33, 25, 17, 9,
    //                1, 58, 50, 42, 34, 26, 18,
    //                10, 2, 59, 51, 43, 35, 27,
    //                19, 11, 3, 60, 52, 44, 36,
    //                63, 55, 47, 39, 31, 23, 15,
    //                7, 62, 54, 46, 38, 30, 22,
    //                14, 6, 61, 53, 45, 37, 29,
    //                21, 13, 5, 28, 20, 12, 4};
    uint64_t output;
    // // int output2[64];

    // printf("\n");
    // printf("\n");

    // for (int i = 0; i < 8; i++)
    // {
    //     printf("%d", input[i]);
    // }

    // printf("\n");
    // printf("\n");

    // // for (int i = 0; i < 56; i++)
    // // {
    // //     printf("%d", key[i]);
    // // }

    // // printf("\n");
    // // printf("\n");

    // // // // Use generate_keys for encryption
    // generate_keys(key, round_keys); // Generate the round keys for encryption

    // for (int i = 0; i < 16; i++)
    // {
    //     printf("Round %d Key: ", i + 1);

    //     // Print each key in binary (48 bits per round)
    //     for (int j = 0; j < 48; j++)
    //     {
    //         printf("%d", round_keys[i][j]);
    //     }

    //     printf("\n");
    // }

    // Generate the keys
    generate_keys(&key, round_keys);

    // Print the round keys (for verification)
    for (int i = 0; i < 16; i++)
    {
        printf("Round %d key: ", i + 1);
        for (int j = 0; j < 6; j++)
        {
            printf("%02X ", round_keys[i][j]);
        }
        printf("\n");
    }

    // printf("Original Data:\n");
    // for (int i = 0; i < 64; i++)
    //     printf("%d ", input[i]);
    // printf("\n");
    // printf("\n");

    // for (int i = 0; i < 8; i++)
    // {
    //     printf("%d", input[i]);
    // }
    // printf("\n");
    // printf("\n");

    // for (int i = 0; i < 8; i++)
    // {
    //     printf("%d", input[i]);
    // }

    // // Encrypt the data
    desE(&input, &output, round_keys);

    // Printing the 64-bit value as hexadecimal
    printf("The value of input in hexadecimal: 0x%llx\n", output);

    // Alternatively, printing the 64-bit value in decimal
    printf("The value of input in decimal: %llu\n", output);

    // printf("Encrypted Data:\n");
    // for (int i = 0; i < 8; i++)
    //     printf("%d ", output[i]);
    // printf("\n");

    // // Use generate_keysD for decryption (reverse order of round keys)
    // generate_keysD(key, round_keys); // Generate the round keys for decryption

    // // Decrypt the data
    // desD(output, output2, round_keys);

    // printf("Decrypted Data:\n");
    // for (int i = 0; i < 64; i++)
    //     printf("%d ", output2[i]);
    // printf("\n");

    // Open input and output files
    // FILE *input_file = fopen("file.txt", "rb");
    // FILE *output_file = fopen("output.txt", "wb");

    // // Define a 64-bit key (this could come from user input or a fixed key)
    // unsigned char key2[7] = {0x13, 0x89, 0x56, 0x7F, 0xA4, 0x23, 0x9B}; // 56-bit key (7 bytes)

    // printf("\n");
    // printf("\n");
    // printf("Total size of array in bytes: %zu\n", sizeof(key2));
    // printf("Size of one element (int): %zu\n", sizeof(key2[0]));
    // printf("Number of elements: %zu\n", sizeof(key2) / sizeof(key2[0]));

    // // Encrypt the file
    // encrypt_file(input_file, output_file, key);

    // Close files
    // fclose(input_file);
    // fclose(output_file);

    // return 0;

    return 0;
}
