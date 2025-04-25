#include <stdio.h>
// #include <stdlib.h>
#include <stdint.h>
#include <string.h>

// Round constants
uint32_t rcon[10] = {0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000,
                     0x20000000, 0x40000000, 0x80000000, 0x1B000000, 0x36000000};

// uint8_t arrays[16] = {0x02, 0x03, 0x01, 0x01,
//                       0x01, 0x02, 0x03, 0x01,
//                       0x01, 0x01, 0x02, 0x03,
//                       0x03, 0x01, 0x01, 0x02};

// uint8_t arrays2[16] = {0x0e, 0x0b, 0x0d, 0x09,
//                        0x09, 0x0e, 0x0b, 0x0d,
//                        0x0d, 0x09, 0x0e, 0x0b,
//                        0x0b, 0x0d, 0x09, 0x0e};

uint8_t Nb = 4;

// AES S-box
uint8_t sbox[256] = {0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
                     0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
                     0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
                     0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
                     0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
                     0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
                     0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
                     0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
                     0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
                     0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
                     0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
                     0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
                     0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
                     0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
                     0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
                     0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16};

uint8_t sbox_inv[256] = {0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
                         0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
                         0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
                         0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
                         0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
                         0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
                         0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
                         0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
                         0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
                         0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
                         0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
                         0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
                         0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
                         0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
                         0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
                         0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d};

void print_uint8_array_hex_matrix(const uint8_t arr[16])
{
    for (int i = 0; i < 16; i++)
    {
        if ((i) % 4 == 0)
        {
            printf("\n");
        }
        printf("|%02x|", arr[i]);
    }
}

void print_uint8_array_hex(const uint8_t arr[16])
{
    for (int i = 0; i < 16; i++)
    {
        printf("%02x", arr[i]);
    }
}

// void print_uint32_array_hex_matrix(const uint32_t arr[4])
// {
//     for (int i = 0; i < 4; i++)
//     {
//         print_uint8_array_hex_matrix()
//     }
// }

void print_uint32_array_hex(const uint32_t arr[4])
{
    printf("\n");
    for (int i = 0; i < 4; i++)
    {
        printf("%08x", arr[i]);
    }
    // printf("\n");
}

// Left shift function
uint32_t rotateWord(uint32_t word)
{
    return (word << 8) | (word >> 24);
}

// Sbox substitution. Function that takes a four-byte input word and applies the
// S-box to each of the four bytes to produce an output word.
uint32_t substituteWord(uint32_t word) // Involution function (1 to 1)
{
    // Extract the four bytes
    uint8_t b0 = (uint8_t)(word >> 24);
    uint8_t b1 = (uint8_t)(word >> 16);
    uint8_t b2 = (uint8_t)(word >> 8);
    uint8_t b3 = (uint8_t)word;

    // Perform S-box substitution on each byte
    uint8_t row, col;

    // Byte 0
    row = b0 >> 4;               // Get the upper 4 bits (left hex digit)
    col = b0 & 0x0F;             // Get the lower 4 bits (right hex digit)
    b0 = sbox[(row << 4) | col]; // Equivalent to sbox[row * 16 + col]

    // Byte 1
    row = b1 >> 4;
    col = b1 & 0x0F;
    b1 = sbox[(row << 4) | col];

    // Byte 2
    row = b2 >> 4;
    col = b2 & 0x0F;
    b2 = sbox[(row << 4) | col];

    // Byte 3
    row = b3 >> 4;
    col = b3 & 0x0F;
    b3 = sbox[(row << 4) | col];

    // Recombine the substituted bytes into a uint32_t
    uint32_t substitutedWord = ((uint32_t)b0 << 24) |
                               ((uint32_t)b1 << 16) |
                               ((uint32_t)b2 << 8) |
                               (uint32_t)b3;

    return substitutedWord;
}

// g_function() in key expansion process
uint32_t g_function(uint32_t word, uint8_t iter)
{
    // Step 1. RotWord() (Left shift)
    word = rotateWord(word);

    // Step 2. Sbox substitution.
    word = substituteWord(word); // (sbox permutation)

    // Step 3. XOR
    word = word ^ rcon[iter];

    return word;
}

// Perform multiplication in GF(2^8)
uint8_t xtime(uint8_t x)
{
    return (x << 1) ^ ((x >> 7) * 0x1b); // XTIMES logic
}

uint8_t mul(uint8_t x, uint8_t y)
{
    uint8_t result = 0;
    while (y)
    {
        if (y & 1)
            result ^= x;
        x = xtime(x);
        y >>= 1;
    }
    return result;
}

// Mix one column of the state
void mixSingleColumn(uint8_t *r)
{
    uint8_t t = r[0] ^ r[1] ^ r[2] ^ r[3];
    uint8_t tmp = r[0];

    uint8_t r0 = r[0] ^ t ^ xtime(r[0] ^ r[1]);
    uint8_t r1 = r[1] ^ t ^ xtime(r[1] ^ r[2]);
    uint8_t r2 = r[2] ^ t ^ xtime(r[2] ^ r[3]);
    uint8_t r3 = r[3] ^ t ^ xtime(r[3] ^ tmp);

    r[0] = r0;
    r[1] = r1;
    r[2] = r2;
    r[3] = r3;
}

void invMixSingleColumn(uint8_t *r)
{
    uint8_t a0 = r[0], a1 = r[1], a2 = r[2], a3 = r[3];
    r[0] = mul(a0, 0x0e) ^ mul(a1, 0x0b) ^ mul(a2, 0x0d) ^ mul(a3, 0x09);
    r[1] = mul(a0, 0x09) ^ mul(a1, 0x0e) ^ mul(a2, 0x0b) ^ mul(a3, 0x0d);
    r[2] = mul(a0, 0x0d) ^ mul(a1, 0x09) ^ mul(a2, 0x0e) ^ mul(a3, 0x0b);
    r[3] = mul(a0, 0x0b) ^ mul(a1, 0x0d) ^ mul(a2, 0x09) ^ mul(a3, 0x0e);
}

// Key expansion function
void keyExpansion(uint8_t Nr, uint8_t Nk, uint8_t key[4 * Nk], uint32_t round_keys[Nr + 1][Nb])
{
    uint32_t w[Nb * (Nr + 1)];
    uint8_t i = 0;
    uint32_t temp;
    uint8_t iter = 0;
    uint8_t iter2 = 0;

    // Step 1: Copy the key into the first Nk words of w
    for (i = 0; i < Nk; ++i)
    {
        w[i] = (key[4 * i] << 24) |
               (key[4 * i + 1] << 16) |
               (key[4 * i + 2] << 8) |
               (key[4 * i + 3]);
    }

    while (i < Nb * (Nr + 1))
    {
        temp = w[i - 1];
        if (i % Nk == 0)
        {
            temp = g_function(temp, iter);
            iter++;
        }
        else if ((Nk > 6) && (i % Nk == 4))
        {
            temp = substituteWord(temp);
        }

        w[i] = w[i - Nk] ^ temp;

        // printf("\nw(%d) =  %08x", i, w[i]);
        i++;
    }

    // Step 3: Organize words from w into round_keys array
    for (int j = 0; j < Nr + 1; j++) // Iterate through all round keys (including the initial key)
    {
        for (int i = 0; i < 4; i++) // i = column index
        {
            round_keys[j][i] =
                ((w[j * 4 + 0] >> (24 - i * 8)) & 0xFF) << 24 |
                ((w[j * 4 + 1] >> (24 - i * 8)) & 0xFF) << 16 |
                ((w[j * 4 + 2] >> (24 - i * 8)) & 0xFF) << 8 |
                ((w[j * 4 + 3] >> (24 - i * 8)) & 0xFF);
        }
    }
}

// Add round key method
void addRoundKey(uint8_t Nk, uint8_t Nr, uint8_t Nb, uint8_t input[16], uint32_t roundkey[4])
{
    uint32_t w[4];
    int i;

    // Step 1: Copy the key into the first Nk words of w
    for (i = 0; i < 4; ++i)
    {
        w[i] = (input[i + 0 * 4] << 24) | // Column i, Row 0
               (input[i + 1 * 4] << 16) | // Column i, Row 1
               (input[i + 2 * 4] << 8) |  // Column i, Row 2
               (input[i + 3 * 4]);        // Column i, Row 3
    }

    for (i = 0; i < 4; i++)
    {
        w[i] = w[i] ^ roundkey[i];
    }

    // Write the modified words back to the input array (maintaining endianness)
    for (i = 0; i < 4; ++i)
    {
        input[i * 4 + 0] = (uint8_t)(w[i] >> 24);
        input[i * 4 + 1] = (uint8_t)(w[i] >> 16);
        input[i * 4 + 2] = (uint8_t)(w[i] >> 8);
        input[i * 4 + 3] = (uint8_t)w[i];
    }
}

// Add round key method
void addRoundKeyTwo(uint8_t Nk, uint8_t Nr, uint8_t Nb, uint8_t input[16], uint32_t roundkey[4])
{
    uint32_t w[4];
    int i;

    // Step 1: Copy the key into the first Nk words of w
    for (i = 0; i < 4; ++i)
    {
        w[i] = (input[4 * i] << 24) |
               (input[4 * i + 1] << 16) |
               (input[4 * i + 2] << 8) |
               (input[4 * i + 3]);
    }

    for (i = 0; i < 4; i++)
    {
        w[i] = w[i] ^ roundkey[i];
    }

    // Write the modified words back to the input array (maintaining endianness)
    for (i = 0; i < 4; ++i)
    {
        input[i * 4 + 0] = (uint8_t)(w[i] >> 24);
        input[i * 4 + 1] = (uint8_t)(w[i] >> 16);
        input[i * 4 + 2] = (uint8_t)(w[i] >> 8);
        input[i * 4 + 3] = (uint8_t)w[i];
    }
}

// Substitute bytes method
void subBytes(uint8_t input[16]) // Involution function (1 to 1)
{
    // Perform S-box substitution on each byte
    uint8_t row, col;

    for (int i = 0; i < 16; i++)
    {
        // Byte 0
        row = input[i] >> 4;               // Get the upper 4 bits (left hex digit)
        col = input[i] & 0x0F;             // Get the lower 4 bits (right hex digit)
        input[i] = sbox[(row << 4) | col]; // Equivalent to sbox[row * 16 + col]
    }
}

// Substitute bytes method
void invSubBytes(uint8_t input[16]) // Involution function (1 to 1)
{
    // Perform S-box substitution on each byte
    uint8_t row, col;

    for (int i = 0; i < 16; i++)
    {
        // Byte 0
        row = input[i] >> 4;                   // Get the upper 4 bits (left hex digit)
        col = input[i] & 0x0F;                 // Get the lower 4 bits (right hex digit)
        input[i] = sbox_inv[(row << 4) | col]; // Equivalent to sbox[row * 16 + col]
    }
}

// Shift rows method
void shiftRows(uint8_t wordd[16])
{
    uint8_t temp;

    // Row 1: Shift left by 1 byte
    temp = wordd[4];
    wordd[4] = wordd[5];
    wordd[5] = wordd[6];
    wordd[6] = wordd[7];
    wordd[7] = temp;

    // Row 2: Shift left by 2 bytes
    temp = wordd[8];
    wordd[8] = wordd[10];
    wordd[10] = temp;
    temp = wordd[9];
    wordd[9] = wordd[11];
    wordd[11] = temp;

    // Row 3: Shift left by 3 bytes
    temp = wordd[12];
    wordd[12] = wordd[15];
    wordd[15] = wordd[14];
    wordd[14] = wordd[13];
    wordd[13] = temp;
}

// Shift rows method
void invShiftRows(uint8_t wordd[16])
{
    uint8_t temp;

    // Row 1: Shift left by 1 byte
    temp = wordd[7];
    wordd[7] = wordd[6];
    wordd[6] = wordd[5];
    wordd[5] = wordd[4];
    wordd[4] = temp;

    // Row 2: Shift left by 2 bytes
    temp = wordd[8];
    wordd[8] = wordd[10];
    wordd[10] = temp;
    temp = wordd[9];
    wordd[9] = wordd[11];
    wordd[11] = temp;

    // Row 3: Shift right by 3 bytes
    temp = wordd[15];
    wordd[15] = wordd[12];
    wordd[12] = wordd[13];
    wordd[13] = wordd[14];
    wordd[14] = temp;
}

// Mix Columns method
void mixColumns(uint8_t input[16]) // Involution function (1 to 1)
{
    // Split columns
    for (int i = 0; i < 4; i++)
    {
        uint8_t col[4];
        col[0] = input[i + 0 * 4];
        col[1] = input[i + 1 * 4];
        col[2] = input[i + 2 * 4];
        col[3] = input[i + 3 * 4];

        mixSingleColumn(col);

        input[i + 0 * 4] = col[0];
        input[i + 1 * 4] = col[1];
        input[i + 2 * 4] = col[2];
        input[i + 3 * 4] = col[3];
    }
}

// Mix Columns method
void invMixColumns(uint8_t input[16]) // Involution function (1 to 1)
{
    // Split columns
    for (int i = 0; i < 4; i++)
    {
        uint8_t col[4];
        col[0] = input[i + 0 * 4];
        col[1] = input[i + 1 * 4];
        col[2] = input[i + 2 * 4];
        col[3] = input[i + 3 * 4];

        invMixSingleColumn(col);

        input[i + 0 * 4] = col[0];
        input[i + 1 * 4] = col[1];
        input[i + 2 * 4] = col[2];
        input[i + 3 * 4] = col[3];
    }
}

/*
AES-128 (128 bit key size, 128 bit plaintext size,
        10 rounds, 128 bit round key size

44 words
*/
void aes_128_encrypt(uint8_t input[16], uint8_t key[16])
{
    uint8_t Nr = 10;
    uint8_t Nk = 4;
    uint32_t round_keys[Nr + 1][Nb];

    printf("\nInput to encrypt:\n");
    print_uint8_array_hex_matrix(input);

    // Generate round keys and print them
    keyExpansion(Nr, Nk, key, round_keys);

    printf("\n");

    for (int x = 0; x < Nr + 1; x++)
    {
        printf("\nRound Key(%d): ", x + 1);
        print_uint32_array_hex(round_keys[x]);
    }

    printf("\n");

    // Initial add round key
    addRoundKey(Nk, Nr, Nb, input, round_keys[0]);

    printf("\nAfter Initial add round key: ");
    print_uint8_array_hex(input);

    printf("\n");

    // For loop for Nr - 1 rounds
    for (int l = 0; l < Nr - 1; l++)
    {
        // subBytes()
        subBytes(input);

        // shiftRows()
        shiftRows(input);

        // mixCOlumns()
        mixColumns(input);

        // addRoundKey()
        addRoundKeyTwo(Nk, Nr, Nb, input, round_keys[l + 1]);
    }

    // subBytes()
    subBytes(input);

    // shiftRows()
    shiftRows(input);

    addRoundKeyTwo(Nk, Nr, Nb, input, round_keys[Nr]);

    uint8_t output[16] = {0};

    for (int h = 0; h < 4; h++)
    {
        output[h * 4 + 0] = input[0 * 4 + h];
        output[h * 4 + 1] = input[1 * 4 + h];
        output[h * 4 + 2] = input[2 * 4 + h];
        output[h * 4 + 3] = input[3 * 4 + h];
    }

    printf("\nCiphertext:\n");
    print_uint8_array_hex(input);
}

void aes_128_decrypt(uint8_t input[16], uint8_t key[16])
{
    uint8_t Nr = 10;
    uint8_t Nk = 4;
    uint32_t round_keys[Nr + 1][Nb];

    printf("\nInput to decrypt:\n");
    print_uint8_array_hex(input);

    // Generate round keys and print them
    keyExpansion(Nr, Nk, key, round_keys);

    // Initial add round key
    addRoundKeyTwo(Nk, Nr, Nb, input, round_keys[10]);

    // For loop for Nr - 1 rounds
    for (int l = Nr; l > 1; l--)
    {
        // shiftRows()
        invShiftRows(input);

        // subBytes()
        invSubBytes(input);

        // addRoundKey()
        addRoundKeyTwo(Nk, Nr, Nb, input, round_keys[l - 1]);

        // mixCOlumns()
        invMixColumns(input);
    }

    // shiftRows()
    invShiftRows(input);

    // subBytes()
    invSubBytes(input);

    addRoundKeyTwo(Nk, Nr, Nb, input, round_keys[0]);

    uint8_t output[16] = {0};

    for (int h = 0; h < 4; h++)
    {
        output[h * 4 + 0] = input[0 * 4 + h];
        output[h * 4 + 1] = input[1 * 4 + h];
        output[h * 4 + 2] = input[2 * 4 + h];
        output[h * 4 + 3] = input[3 * 4 + h];
    }

    memcpy(input, output, 16);

    printf("\nCiphertext:\n");
    print_uint8_array_hex(output);
}

/*
AES-192 (192 bit key size, 128 bit plaintext size,
        12 rounds, 128 bit round key size

52 words
*/
void aes_192_encrypt(uint8_t input[16], uint8_t key[24])
{
    uint8_t Nr = 12;
    uint8_t Nk = 6;

    uint32_t round_keys[Nr + 1][Nb];

    printf("\nInput to encrypt:\n");
    print_uint8_array_hex(input);

    // Generate round keys and print them
    keyExpansion(Nr, Nk, key, round_keys);

    // Initial add round key
    addRoundKey(Nk, Nr, Nb, input, round_keys[0]);

    // For loop for Nr - 1 rounds
    for (int l = 0; l < Nr - 1; l++)
    {
        // subBytes()
        subBytes(input);

        // shiftRows()
        shiftRows(input);

        // mixCOlumns()
        mixColumns(input);

        // addRoundKey()
        addRoundKeyTwo(Nk, Nr, Nb, input, round_keys[l + 1]);
    }

    // subBytes()
    subBytes(input);

    // shiftRows()
    shiftRows(input);

    addRoundKeyTwo(Nk, Nr, Nb, input, round_keys[Nr]);

    uint8_t output[16] = {0};

    for (int h = 0; h < 4; h++)
    {
        output[h * 4 + 0] = input[0 * 4 + h];
        output[h * 4 + 1] = input[1 * 4 + h];
        output[h * 4 + 2] = input[2 * 4 + h];
        output[h * 4 + 3] = input[3 * 4 + h];
    }

    printf("\nCiphertext:\n");
    print_uint8_array_hex(output);
}

void aes_192_decrypt(uint8_t input[16], uint8_t key[24])
{
    uint8_t Nr = 12;
    uint8_t Nk = 6;

    uint32_t round_keys[Nr + 1][Nb];

    printf("\nInput to decrypt:\n");
    print_uint8_array_hex(input);

    // Generate round keys and print them
    keyExpansion(Nr, Nk, key, round_keys);

    // Initial add round key
    addRoundKeyTwo(Nk, Nr, Nb, input, round_keys[10]);

    // For loop for Nr - 1 rounds
    for (int l = Nr; l > 1; l--)
    {
        // shiftRows()
        invShiftRows(input);

        // subBytes()
        invSubBytes(input);

        // addRoundKey()
        addRoundKeyTwo(Nk, Nr, Nb, input, round_keys[l - 1]);

        // mixCOlumns()
        invMixColumns(input);
    }

    // shiftRows()
    invShiftRows(input);

    // subBytes()
    invSubBytes(input);

    addRoundKeyTwo(Nk, Nr, Nb, input, round_keys[0]);

    uint8_t output[16] = {0};

    for (int h = 0; h < 4; h++)
    {
        output[h * 4 + 0] = input[0 * 4 + h];
        output[h * 4 + 1] = input[1 * 4 + h];
        output[h * 4 + 2] = input[2 * 4 + h];
        output[h * 4 + 3] = input[3 * 4 + h];
    }

    printf("\nCiphertext:\n");
    print_uint8_array_hex(output);
}

/*
AES-256 (256 bit key size, 128 bit plaintext size,
        14 rounds, 128 bit round key size

60 words
*/
void aes_256_encrypt(uint8_t input[16], uint8_t key[32])
{
    uint8_t Nr = 14;
    uint8_t Nk = 8;

    uint32_t round_keys[Nr + 1][Nb];

    printf("\nInput to encrypt:\n");
    print_uint8_array_hex(input);

    // Generate round keys and print them
    keyExpansion(Nr, Nk, key, round_keys);

    for (int k = 0; k < Nr + 1; k++)
    {
        print_uint32_array_hex(round_keys[k]);
    }

    // Initial add round key
    addRoundKey(Nk, Nr, Nb, input, round_keys[0]);

    // For loop for Nr - 1 rounds
    for (int l = 0; l < Nr - 1; l++)
    {
        // subBytes()
        subBytes(input);

        // shiftRows()
        shiftRows(input);

        // mixCOlumns()
        mixColumns(input);

        // addRoundKey()
        addRoundKeyTwo(Nk, Nr, Nb, input, round_keys[l + 1]);
    }

    // subBytes()
    subBytes(input);

    // shiftRows()
    shiftRows(input);

    addRoundKeyTwo(Nk, Nr, Nb, input, round_keys[Nr]);

    // uint8_t output[16] = {0};

    // for (int h = 0; h < 4; h++)
    // {
    //     output[h * 4 + 0] = input[0 * 4 + h];
    //     output[h * 4 + 1] = input[1 * 4 + h];
    //     output[h * 4 + 2] = input[2 * 4 + h];
    //     output[h * 4 + 3] = input[3 * 4 + h];
    // }

    printf("\nCiphertext:\n");
    print_uint8_array_hex(input);
}

void aes_256_decrypt(uint8_t input[16], uint8_t key[32])
{
    uint8_t Nr = 14;
    uint8_t Nk = 8;

    uint32_t round_keys[Nr + 1][Nb];

    printf("\nInput to decrypt:\n");
    print_uint8_array_hex(input);

    // Generate round keys and print them
    keyExpansion(Nr, Nk, key, round_keys);

    // Initial add round key
    addRoundKeyTwo(Nk, Nr, Nb, input, round_keys[14]);

    // For loop for Nr - 1 rounds
    for (int l = Nr; l > 1; l--)
    {
        // shiftRows()
        invShiftRows(input);

        // subBytes()
        invSubBytes(input);

        // addRoundKey()
        addRoundKeyTwo(Nk, Nr, Nb, input, round_keys[l - 1]);

        // mixCOlumns()
        invMixColumns(input);
    }

    // shiftRows()
    invShiftRows(input);

    // subBytes()
    invSubBytes(input);

    addRoundKeyTwo(Nk, Nr, Nb, input, round_keys[0]);

    uint8_t output[16] = {0};

    for (int h = 0; h < 4; h++)
    {
        output[h * 4 + 0] = input[0 * 4 + h];
        output[h * 4 + 1] = input[1 * 4 + h];
        output[h * 4 + 2] = input[2 * 4 + h];
        output[h * 4 + 3] = input[3 * 4 + h];
    }

    printf("\nCiphertext:\n");
    print_uint8_array_hex(output);
}

// Main
int main(int argc, char *argv[])
{
    // 128-bit ousbox_output block (message data) as an array of integers (8 bytes)
    uint8_t input[16] = {0x79, 0x6f, 0x77, 0x68,
                         0x61, 0x74, 0x75, 0x70,
                         0x6d, 0x79, 0x64, 0x61,
                         0x77, 0x67, 0x67, 0x67}; // yowhatupmydawggg

    uint8_t input2[16] = {0x32, 0x43, 0xf6, 0xa8,
                          0x88, 0x5a, 0x30, 0x8d,
                          0x31, 0x31, 0x98, 0xa2,
                          0xe0, 0x37, 0x07, 0x34};

    uint8_t input3[16] = {0x39, 0x02, 0xdc, 0x19,
                          0x25, 0xdc, 0x11, 0x6a,
                          0x84, 0x09, 0x85, 0x0b,
                          0x1d, 0xfb, 0x97, 0x32};

    uint8_t cipher[16] = {0x30, 0x21, 0x61, 0x3a,
                          0x97, 0x3e, 0x58, 0x2f,
                          0x4a, 0x29, 0x23, 0x41,
                          0x37, 0xae, 0xc4, 0x94};

    uint8_t cipher2[16] = {0x30, 0x97, 0x4a, 0x37,
                           0x21, 0x3e, 0x29, 0xae,
                           0x61, 0x58, 0x23, 0xc4,
                           0x3a, 0x2f, 0x41, 0x94};

    uint8_t key[16] = {0x68, 0x65, 0x6c, 0x6c,
                       0x6f, 0x63, 0x6f, 0x6d,
                       0x72, 0x61, 0x64, 0x65,
                       0x67, 0x6f, 0x6f, 0x64}; // hellocomradegood

    uint8_t key2[16] = {0x2b, 0x7e, 0x15, 0x16,
                        0x28, 0xae, 0xd2, 0xa6,
                        0xab, 0xf7, 0x15, 0x88,
                        0x09, 0xcf, 0x4f, 0x3c};

    uint8_t key3[32] = {0x68, 0x65, 0x6c, 0x6c,
                        0x6f, 0x63, 0x6f, 0x6d,
                        0x72, 0x61, 0x64, 0x65,
                        0x67, 0x6f, 0x6f, 0x64,
                        0x68, 0x65, 0x6c, 0x6c,
                        0x6f, 0x63, 0x6f, 0x6d,
                        0x6d, 0x61, 0x6e, 0x64,
                        0x65, 0x72, 0x6f, 0x62}; // hellocomradegoodhellocommanderob

    uint8_t key4[32] = {0x60, 0x3d, 0xeb, 0x10,
                        0x15, 0xca, 0x71, 0xbe,
                        0x2b, 0x73, 0xae, 0xf0,
                        0x85, 0x7d, 0x77, 0x81,
                        0x1f, 0x35, 0x2c, 0x07,
                        0x3b, 0x61, 0x08, 0xd7,
                        0x2d, 0x98, 0x10, 0xa3,
                        0x09, 0x14, 0xdf, 0xf4};

    uint8_t key5[32] = {0x2b, 0x7e, 0x15, 0x16,
                        0x28, 0xae, 0xd2, 0xa6,
                        0xab, 0xf7, 0x15, 0x88,
                        0x09, 0xcf, 0x4f, 0x3c,
                        0x2b, 0x7e, 0x15, 0x16,
                        0x28, 0xae, 0xd2, 0xa6,
                        0xab, 0xf7, 0x15, 0x88,
                        0x09, 0xcf, 0x4f, 0x3c};

    if (argc == 4)
    {
        uint8_t mode;
        if (strcmp(argv[1], "128") == 0)
        {
            if (strcmp(argv[2], "e") == 0)
            {
                FILE *file = fopen("file.txt", "r");
                FILE *file2 = fopen("file_encrypted.txt", "w+");
                size_t bytes_read;
                uint8_t buffer[16];

                if (file == NULL || file2 == NULL)
                {
                    perror("Error opening file");
                    return 1;
                }

                printf("You chose AES-128 bit encrypt.\n");
                while ((bytes_read = fread(buffer, sizeof(uint8_t), 16, file)) > 0)
                {
                    aes_128_encrypt(buffer, key2);
                    fwrite(buffer, sizeof(uint8_t), 16, file2);
                }
            }
            else if (strcmp(argv[2], "d") == 0)
            {

                FILE *file2 = fopen("file_encrypted.txt", "r");
                FILE *file3 = fopen("file_decrypted.txt", "w+");
                size_t bytes_read2;
                uint8_t cipher_block[16];

                if (file2 == NULL || file3 == NULL)
                {
                    perror("Error opening file");
                    return 1;
                }

                printf("You chose AES-128 bit decrypt.\n");
                while ((bytes_read2 = fread(cipher_block, sizeof(uint8_t), 16, file2)) > 0)
                {
                    aes_128_decrypt(cipher_block, key2);
                    fwrite(cipher_block, sizeof(uint8_t), 16, file3);
                }
            }
            else
            {
                printf("Not valid mode. enter 'e' or 'd'.\n");
            }
        }
        else if (strcmp(argv[1], "192") == 0)
        {
            if (strcmp(argv[2], "e") == 0)
            {
                printf("You chose AES-192 bit encrypt.\n");
                // aes_192_encrypt(input2, key2);
            }
            else if (strcmp(argv[2], "d") == 0)
            {
                printf("You chose AES-192 bit decrypt.\n");
                // aes_192_decrypt(input2, key2);
            }
            else
            {
                printf("Not valid mode. enter 'e' or 'd'.\n");
            }
        }
        else if (strcmp(argv[1], "256") == 0)
        {
            if (strcmp(argv[2], "e") == 0)
            {
                printf("You chose AES-256 bit encrypt.\n");
                aes_256_encrypt(input2, key4);
            }
            else if (strcmp(argv[2], "d") == 0)
            {
                printf("You chose AES-256 bit decrypt.\n");
                aes_256_decrypt(cipher2, key4);
            }
            else
            {
                printf("Not valid mode. enter 'e' or 'd'.\n");
            }
        }
        else
        {
            printf("Not valid mode. Enter either '128', '192', or '256'.\n");
        }
    }
    else if (argc == 3)
    { // Make sure there is at least one command-line argument
        uint8_t mode;
        if (strcmp(argv[1], "128") == 0)
        {
            if (strcmp(argv[2], "e") == 0)
            {
                printf("You chose AES-128 bit encrypt.\n");
                aes_128_encrypt(input, key);
            }
            else if (strcmp(argv[2], "d") == 0)
            {
                aes_128_decrypt(input3, key2);
            }
            else
            {
                printf("Not valid mode. enter 'e' or 'd'.\n");
            }
        }
        else if (strcmp(argv[1], "192") == 0)
        {
            if (strcmp(argv[2], "e") == 0)
            {
                printf("You chose AES-192 bit encrypt.\n");
                // aes_192_encrypt(input2, key2);
            }
            else if (strcmp(argv[2], "d") == 0)
            {
                printf("You chose AES-192 bit decrypt.\n");
                // aes_192_decrypt(input2, key2);
            }
            else
            {
                printf("Not valid mode. enter 'e' or 'd'.\n");
            }
        }
        else if (strcmp(argv[1], "256") == 0)
        {
            if (strcmp(argv[2], "e") == 0)
            {
                printf("You chose AES-256 bit encrypt.\n");
                aes_256_encrypt(input2, key5);
            }
            else if (strcmp(argv[2], "d") == 0)
            {
                printf("You chose AES-256 bit decrypt.\n");
                aes_256_decrypt(cipher2, key4);
            }
            else
            {
                printf("Not valid mode. enter 'e' or 'd'.\n");
            }
        }
        else
        {
            printf("Not valid mode. Enter either '128', '192', or '256'.\n");
        }
    }
    else
    {
        printf("Usage: aes.exe <128/192/256> <e/d> <input file> <output file> \n");
    }

    return 0;
}
