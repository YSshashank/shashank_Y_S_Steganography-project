#include "decode.h"
#include "common.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Step 1: Read and validate command line arguments
DecodeStatus read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Step 1: Check if stego image has .bmp extension
    decInfo->stego_image_fname = argv[2];
    char *dot = strrchr(decInfo->stego_image_fname, '.');

    if (dot == NULL || strcmp(dot, ".bmp") != 0)
    {
        printf("Destination image file must have .bmp extension.\n");
        return d_failure;
    }

    // Step 2: Handle output filename
    if (argv[3] != NULL)
    {
        // Copy to a buffer before modification
        static char imageBuffer[100];

        strcpy(imageBuffer, argv[3]);

        char *dot2 = strrchr(imageBuffer, '.');

        if (dot2 != NULL && strcmp(dot2, ".txt") == 0)
        {
            // Remove extension safely
            *dot2 = '\0';
        }

        // Store filename (without extension)
        decInfo->secret_fname = imageBuffer;
    }
    else
    {
        // Default output filename
        decInfo->secret_fname = "decoded";
    }

    return d_success;
}

// Step 2: Open decode files
DecodeStatus open_decode_files(DecodeInfo *decInfo)
{
    // Stego Image file
    decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "rb");
    // Do Error handling
    if (decInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);

        return d_failure;
    }
    // No failure return d_success
    return d_success;
}

// Step 3: Decode magic string
DecodeStatus decode_magic_string(const char *expected_magic_string, DecodeInfo *decInfo)
{
    // Find the Length of expected_magic_string
    int len = strlen(expected_magic_string);

    char imageBuffer[8];
    // safe buffer
    char decoded_magic_string[16]; // safe buffer

    for (int i = 0; i < len; i++)
    {
        // Read 8 bytes from stego_image
        fread(imageBuffer, sizeof(char), 8, decInfo->fptr_stego_image);

        // Decode 8 byte from lsb
        decode_byte_from_lsb(&decoded_magic_string[i], imageBuffer);
    }

    decoded_magic_string[len] = '\0';

    // compare decoded_magic_string and  expected_magic_string are equal or not
    if (strcmp(decoded_magic_string, expected_magic_string) == 0)
    {
        // printf("Decoded Magic String : %s\n", decoded_magic_string);
        return d_success;
    }
    else
    {
        return d_failure;
    }
}

// Step 4: Decode size from LSBs
DecodeStatus decode_size_from_lsb(int *size, char *imageBuffer)
{
    *size = 0;
    for (int i = 0; i < 32; i++)
    {
        // Extract LSB of each byte
        char bit = imageBuffer[i] & 0x01;

        // shift direction
        *size |= (bit << i); // LSB-first order
    }
    return d_success;
}

// Step 5 :Decode one byte from 8 bytes of image data
DecodeStatus decode_byte_from_lsb(char *data, char *image_buffer)
{
    *data = 0;
    for (int i = 0; i < 8; i++)
    {
        int bit = image_buffer[i] & 1;
        *data |= (bit << i);
    }
    return e_success;
}

// Step 6: Decode secret file extension size
DecodeStatus decode_secret_file_extn_size(int *size, DecodeInfo *decInfo)
{
    char imageBuffer[32];

    // Read 32 bytes from stego image
    fread(imageBuffer, sizeof(char), 32, decInfo->fptr_stego_image);

    // Decode size from LSBs
    decode_size_from_lsb(size, imageBuffer);

    return d_success;
}

// Step 7: Decode secret file extension
DecodeStatus decode_secret_file_extn(char *file_extn, int size, DecodeInfo *decInfo)
{
    char imageBuffer[8];

    for (int i = 0; i < size; i++)
    {
        fread(imageBuffer, sizeof(char), 8, decInfo->fptr_stego_image);
        decode_byte_from_lsb(&file_extn[i], imageBuffer);
    }

    file_extn[size] = '\0'; // Null-terminate the extension
    printf("Secret file extension decoded : %s\n", file_extn);

    return d_success;
}

// Step 8: Decode secret file size
DecodeStatus decode_secret_file_size(int *file_size, DecodeInfo *decInfo)
{
    char imageBuffer[32];

    // Read 32 bytes from stego image
    fread(imageBuffer, sizeof(char), 32, decInfo->fptr_stego_image);

    // Decode size from LSBs
    decode_size_from_lsb(file_size, imageBuffer);
    return d_success;
}

// Step 9: Decode secret file data
DecodeStatus decode_secret_file_data(DecodeInfo *decInfo, int file_size)
{
    char imageBuffer[8];
    char ch;

    // Open File secret_fname in write mode
    FILE *fptr_output = fopen(decInfo->secret_fname, "wb");

    if (fptr_output == NULL)
    {
        return d_failure;
    }

    for (int i = 0; i < file_size; i++)
    {
        // Read 8 bytes from stego image
        fread(imageBuffer, sizeof(char), 8, decInfo->fptr_stego_image);

        // Decode data from LSBs
        decode_byte_from_lsb(&ch, imageBuffer);

        // write 8 bytes from stego image
        fwrite(&ch, sizeof(char), 1, fptr_output);
    }

    // close file
    fclose(fptr_output);

    return d_success;
}

// Step 10 : Do Decoding
DecodeStatus do_decoding(DecodeInfo *decInfo)
{
    // Step 1 : open the files
    if (open_decode_files(decInfo) == d_success)
    {
        // True print the prompt message
        printf("All files opened success\n");
    }
    else
    {
        // False return d_failure
        return d_failure;
    }

    // Step 2 : skip the header
    fseek(decInfo->fptr_stego_image, 54, SEEK_SET);

    // Step 3 : decode_magic_string(MAGIC_STRING, decInfo) == d_success
    if (decode_magic_string(MAGIC_STRING, decInfo) == d_success)
    {
        // true print the prompt message
        printf("Magic string decoded \n");
    }
    else
    {
        // false return d_failure
        return d_failure;
    }

    // Step 4: Decode the size of the secret file extension
    int extn_size;
    if (decode_secret_file_extn_size(&extn_size, decInfo) == d_success)
    {
        printf("Secret file extension size decoded : %d\n", extn_size);
    }
    else
    {
        return d_failure;
    }

    // Step 5: Decode the data of secret file extension
    char file_extn[10];
    if (decode_secret_file_extn(file_extn, extn_size, decInfo) == d_success)
    {
        printf("Secret file extension decoded : %s\n", file_extn);
    }
    else
    {
        return d_failure;
    }

    // Step 6: Decode the secret file size
    int secret_file_size;
    if (decode_secret_file_size(&secret_file_size, decInfo) == d_success)
    {
        printf("Secret file size decoded : %d\n", secret_file_size);
    }
    else
    {
        return d_failure;
    }

    // Step 7: Prepare output file name
    char *dot = strrchr(decInfo->secret_fname, '.');
    if (dot != NULL)
    {
        *dot = '\0'; // remove existing extension if any
    }

    // Now safely append decoded extension
    sprintf(decInfo->extn_secret_file, "%s%s", decInfo->secret_fname, file_extn);

    // Update pointer to new name
    decInfo->secret_fname = decInfo->extn_secret_file;

    printf("Output file created: %s\n", decInfo->secret_fname);

    // Step 8: Decode the secret file data
    if (decode_secret_file_data(decInfo, secret_file_size) == d_success)
    {
        printf("Secret file data decoded success\n");
    }
    else
    {
        return d_failure;
    }

    fclose(decInfo->fptr_stego_image);

    return d_success;
}
