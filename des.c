#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define BLOCK_SIZE 8
#define KEY_SIZE 8

// Example tables for IP, FP, S-Boxes, etc. (these need to be populated with the actual values)
// int IP[64] = {/* Define initial permutation table */};
int IP[64] = {58, 50, 42, 34, 26, 18, 10, 2,
              60, 52, 44, 36, 28, 20, 12, 4,
              62, 54, 46, 38, 30, 22, 14, 6,
              64, 56, 48, 40, 32, 24, 16, 8,
              57, 49, 41, 33, 25, 17, 9, 1,
              59, 51, 43, 35, 27, 19, 11, 3,
              61, 53, 45, 37, 29, 21, 13, 5,
              63, 55, 47, 39, 31, 23, 15, 7};
// int FP[64] = {/* Define final permutation table */};
int FP[64] = {40, 8, 48, 16, 56, 24, 64, 32,
              39, 7, 47, 15, 55, 23, 63, 31,
              38, 6, 46, 14, 54, 22, 62, 30,
              37, 5, 45, 13, 53, 21, 61, 29,
              36, 4, 44, 12, 52, 20, 60, 28,
              35, 3, 43, 11, 51, 19, 59, 27,
              34, 2, 42, 10, 50, 18, 58, 26,
              33, 1, 41, 9, 49, 17, 57, 25};
// int E[48] = {/* Define expansion table */};
int E[48] = {32, 1, 2, 3, 4, 5,
             4, 5, 6, 7, 8, 9,
             8, 9, 10, 11, 12, 13,
             12, 13, 14, 15, 16, 17,
             16, 17, 18, 19, 20, 21,
             20, 21, 22, 23, 24, 25,
             24, 25, 26, 27, 28, 29,
             28, 29, 30, 31, 32, 1};
int P[32] = {16, 7, 20, 21, 29, 12, 28, 17,
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
int round_keysE[16][48];
int round_keysD[16][48];

// Key rotation schedule
const int key_shifts[16] = {1, 1, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1, 2, 1};

// Key generation constants
// int PC1[56] = {/* Permutation Choice 1 */};
int PC1[56] = {
    57, 49, 41, 33, 25, 17, 9, 20, // First 8 bits from original key (1, 2, 3, 4, ...)
    1, 58, 50, 42, 34, 26, 18, 10,
    2, 59, 51, 43, 35, 27, 19, 11,
    3, 60, 52, 44, 36, 28, 20, 12,
    4, 61, 53, 45, 37, 29, 21, 13,
    5, 62, 54, 46, 38, 30, 22, 14,
    6, 63, 55, 47, 39, 31, 23, 15};
// int PC2[48] = {/* Permutation Choice 2 */};
int PC2[48] = {
    14, 17, 11, 24, 1, 5, // First 6 bits of the 48-bit round key
    3, 28, 15, 6, 21, 10,
    23, 19, 12, 4, 26, 8,
    16, 7, 27, 20, 13, 2,
    41, 52, 31, 37, 47, 55,
    30, 40, 51, 45, 33, 48,
    44, 49, 39, 56, 34, 53,
    46, 42, 50, 36, 29, 32};

void leftShiftArray(int arr[], int size)
{
    if (size <= 1)
    {
        // No shift is needed for an empty or single element array
        return;
    }

    int first = arr[0]; // Store the first element

    // Shift all elements to the left
    for (int i = 1; i < size; i++)
    {
        arr[i - 1] = arr[i];
    }

    // Place the first element in the last position
    arr[size - 1] = first;
}

void rightShiftArray(int arr[], int size)
{
    if (size <= 1)
    {
        // No shift is needed for an empty or single element array
        return;
    }

    int last = arr[size - 1]; // Store the last element

    // Shift all elements to the right
    for (int i = size - 1; i > 0; i--)
    {
        arr[i] = arr[i - 1];
    }

    // Place the last element in the first position
    arr[0] = last;
}

// Functions for Permutation, XOR, S-box substitution, etc.

void permute(int *input, int *table, int length)
{
    int output[length];
    for (int i = 0; i < length; i++)
    {
        output[i] = input[table[i] - 1];
    }
    for (int i = 0; i < length; i++)
    {
        input[i] = output[i];
    }
}

void generate_keysE(int *key, int round_keys[16][48])
{
    // 1. Apply PC-1 to the key to get a 56-bit key
    permute(key, PC1, 56);

    // 2. Split it into two 28-bit halves (C and D)
    int left[28], right[28];
    for (int i = 0; i < 28; i++)
    {
        left[i] = key[i];
        right[i] = key[i + 28];
    }

    // 3. Perform 16 rounds of shifting (C and D) and applying PC-2 to generate the round keys
    for (int i = 0; i < 16; i++)
    {
        for (int j = 1; j <= key_shifts[i]; j++)
        {
            leftShiftArray(left, 28);  // Shift left half (C)
            leftShiftArray(right, 28); // Shift right half (D)
        }

        // After shifting, combine the two halves (C and D) into a 56-bit key
        int combined[56];
        for (int j = 0; j < 28; j++)
        {
            combined[j] = left[j];       // First half (C)
            combined[28 + j] = right[j]; // Second half (D)
        }

        // Apply PC-2 to the combined key to generate a 48-bit round key
        permute(combined, PC2, 48); // PC2 generates a 48-bit round key

        // Store the round key in the round_keys array
        for (int j = 0; j < 48; j++)
        {
            round_keys[i][j] = combined[j];
        }
    }
}

void generate_keysD(int *key, int round_keys[16][48])
{
    // 1. Apply PC-1 to the key to get a 56-bit key
    permute(key, PC1, 56);

    // 2. Split it into two 28-bit halves (C and D)
    int left[28], right[28];
    for (int i = 0; i < 28; i++)
    {
        left[i] = key[i];
        right[i] = key[i + 28];
    }

    // 3. Perform 16 rounds of shifting (C and D) and applying PC-2 to generate the round keys
    for (int i = 15; i >= 0; i--)
    {
        for (int j = 1; j <= key_shifts[i]; j++)
        {
            rightShiftArray(left, 28);  // Shift left half (C)
            rightShiftArray(right, 28); // Shift right half (D)
        }

        // After shifting, combine the two halves (C and D) into a 56-bit key
        int combined[56];
        for (int j = 0; j < 28; j++)
        {
            combined[j] = left[j];       // First half (C)
            combined[28 + j] = right[j]; // Second half (D)
        }

        // Apply PC-2 to the combined key to generate a 48-bit round key
        permute(combined, PC2, 48); // PC2 generates a 48-bit round key

        // Store the round key in the round_keys array
        for (int j = 0; j < 48; j++)
        {
            round_keys[i][j] = combined[j];
        }
    }
}

void f_function(int *right_half, int *round_key, int *output)
{
    // Step 1: Expand the right half using the expansion table
    int expanded[48];
    permute(right_half, E, 48);

    // Step 2: XOR the expanded data with the round key
    for (int i = 0; i < 48; i++)
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

// void f_functionD(int *right_half, int *round_key, int *output)
// {
//     // Step 1: Expand the right half using the expansion table
//     int expanded[48];
//     permute(right_half, E, 48);

//     // Step 2: XOR the expanded data with the round key
//     for (int i = 0; i < 48; i++)
//     {
//         expanded[i] ^= round_key[i];
//     }

//     // Step 3: Apply S-boxes (8 S-boxes, each 6 bits to 4 bits)
//     int sbox_output[8][4]; // This will store the 4-bit output from each S-box
//     for (int i = 0; i < 8; i++)
//     {
//         // Extract the 6-bit block from the expanded array
//         int block[6];
//         for (int j = 0; j < 6; j++)
//         {
//             block[j] = expanded[i * 6 + j];
//         }

//         // Step 3.1: The first and last bits of the 6-bit block represent the row
//         int row = (block[0] << 1) | block[5]; // Row = first bit + last bit
//         // Step 3.2: The middle 4 bits represent the column
//         int col = (block[1] << 3) | (block[2] << 2) | (block[3] << 1) | block[4]; // Column = middle 4 bits

//         // Step 3.3: Look up the S-box value (6-bit input -> 4-bit output)
//         int sbox_value = S[i][row * 16 + col]; // S-box value is at [row * 16 + col]

//         // Step 3.4: Store the 4-bit output of the S-box
//         for (int j = 0; j < 4; j++)
//         {
//             sbox_output[i][j] = (sbox_value >> (3 - j)) & 1; // Extract each bit of the 4-bit output
//         }
//     }

//     // Step 4: Combine the 4-bit outputs of the S-boxes into a 32-bit value
//     for (int i = 0; i < 8; i++)
//     {
//         for (int j = 0; j < 4; j++)
//         {
//             output[i * 4 + j] = sbox_output[i][j];
//         }
//     }

//     // Step 5: Permute the result using the permutation table P
//     permute(output, P, 32);
// }

void desE(int *input, int *output)
{
    // 1. Apply the initial permutation
    permute(input, IP, 64);

    // 2. Split the 64-bit input into two 32-bit halves
    int left[32], right[32];
    for (int i = 0; i < 32; i++)
    {
        left[i] = input[i];
        right[i] = input[i + 32];
    }

    // // 3. Generate 16 round keys
    // generate_keys(key, round_keys);

    // 4. Perform 16 rounds of DES
    for (int round = 0; round < 16; round++)
    {
        // Apply the F-function and XOR with the left half
        int new_left[32], new_right[32];
        f_function(right, round_keysE[round], new_left);

        // Update the halves for the next round
        for (int i = 0; i < 32; i++)
        {
            new_right[i] = left[i] ^ new_left[i];
        }
        for (int i = 0; i < 32; i++)
        {
            left[i] = right[i];
            right[i] = new_right[i];
        }
    }

    // 5. Combine left and right and apply the final permutation
    int final_data[64];
    for (int i = 0; i < 32; i++)
    {
        final_data[i] = left[i];
        final_data[i + 32] = right[i];
    }
    // Final permute
    permute(final_data, FP, 64);

    // 6. Output the final encrypted data
    for (int i = 0; i < 64; i++)
    {
        output[i] = final_data[i];
    }
}

void desD(int *input, int *output)
{
    // 1. Apply the initial permutation
    permute(input, FP, 64);

    // 2. Split the 64-bit input into two 32-bit halves
    int left[32], right[32];
    for (int i = 0; i < 32; i++)
    {
        left[i] = input[i];
        right[i] = input[i + 32];
    }

    // // 3. Generate 16 round keys
    // int round_keys[16][48];
    // generate_keysD(key, round_keys);

    // 4. Perform 16 rounds of DES (but apply round keys in reverse order for decryption)
    for (int round = 15; round >= 0; round--) // Notice the reverse order here
    {
        // Apply the F-function and XOR with the left half
        int new_left[32], new_right[32];
        f_function(right, round_keysD[round], new_left);

        // Update the halves for the next round
        for (int i = 0; i < 32; i++)
        {
            new_right[i] = left[i] ^ new_left[i];
        }
        for (int i = 0; i < 32; i++)
        {
            left[i] = right[i];
            right[i] = new_right[i];
        }
    }

    // 5. Combine left and right and apply the final permutation
    int final_data[64];
    for (int i = 0; i < 32; i++)
    {
        final_data[i] = left[i];
        final_data[i + 32] = right[i];
    }
    permute(final_data, IP, 64);

    // 6. Output the final decrypted data
    for (int i = 0; i < 64; i++)
    {
        output[i] = final_data[i];
    }
}

int main()
{
    // Example data (64 bits) and key (56 bits)
    // int input[64] = {/* 64-bit input data */};
    int input[64] = {62, 54, 46, 38, 30, 22, 14, 6,
                     58, 50, 42, 34, 26, 18, 10, 2,
                     60, 52, 44, 36, 28, 20, 12, 4,
                     64, 56, 48, 40, 32, 24, 16, 8,
                     63, 55, 47, 39, 31, 23, 15, 7,
                     59, 51, 43, 35, 27, 19, 11, 3,
                     61, 53, 45, 37, 29, 21, 13, 5,
                     57, 49, 41, 33, 25, 17, 9, 1};
    // int key[56] = {/* 56-bit key */};
    int key[56] = {57, 49, 41, 33, 25, 17, 9,
                   1, 58, 50, 42, 34, 26, 18,
                   10, 2, 59, 51, 43, 35, 27,
                   19, 11, 3, 60, 52, 44, 36,
                   63, 55, 47, 39, 31, 23, 15,
                   7, 62, 54, 46, 38, 30, 22,
                   14, 6, 61, 53, 45, 37, 29,
                   21, 13, 5, 28, 20, 12, 4};
    int output[64];
    int output2[64];

    for (int i = 0; i < 64; i++)
    {
        printf("%d", input[i]);
    }

    printf("\n");
    printf("\n");

    for (int i = 0; i < 56; i++)
    {
        printf("%d", key[i]);
    }

    printf("\n");
    printf("\n");

    generate_keysE(key, round_keysE);
    generate_keysD(key, round_keysD);

    // Print the round keys for encryption (example, you can modify this as needed)
    for (int i = 0; i < 16; i++)
    {
        printf("Round %d key: ", i + 1);
        for (int j = 0; j < 48; j++)
        {
            printf("%d ", round_keysE[i][j]);
        }
        printf("\n");
    }

    printf("\n");
    printf("\n");

    // Encrypt the data using DES
    desE(input, output);

    // Print the encrypted output (binary or hex format)
    for (int i = 0; i < 64; i++)
    {
        printf("%d ", output[i]);
    }

    printf("\n");
    printf("\n");

    // Print the round keys again in reverse order for decryption (example, you can modify this as needed)
    for (int i = 15; i >= 0; i--)
    {
        printf("Round %d key: ", i);
        for (int j = 0; j < 48; j++)
        {
            printf("%d ", round_keysD[i][j]);
        }
        printf("\n");
    }

    printf("\n");
    printf("\n");

    // Encrypt the data using DES
    desD(output, output2);

    for (int i = 0; i < 64; i++)
    {
        printf("%d ", output2[i]);
    }

    return 0;
}
