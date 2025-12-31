/*
Project Documentation: Image Steganography using LSB Technique

Project Title: Image Steganography using LSB (Least Significant Bit) Technique
Language Used: C
Submitted by: Shashank Y S

Steganography is the art of hiding information within digital media in such a way that the presence of the hidden message
is concealed. This project, Image Steganography using LSB Technique, focuses on embedding a secret text file within a 
BMP image using the Least Significant Bit (LSB) approach.

The application developed in C programming language provides both encoding (hiding data) and decoding (extracting data)
functionalities. It ensures the secret information remains invisible to human eyes while maintaining the original 
appearance of the image.

1. Encoding Process

    The source BMP image, secret text file, and output file name are provided as input.

    The BMP header (54 bytes) is copied unchanged.

    A magic string (identifier) is encoded first to verify during decoding.

    The secret file’s extension, size, and content are encoded bit-by-bit into the LSBs of image pixels.

    The modified image is saved as the stego image.

2. Decoding Process

    The encoded (stego) BMP image is opened.

    The program verifies the magic string to ensure the image is valid.

    The secret file’s extension, size, and data are extracted bit-by-bit from the LSBs.

    The recovered data is written into the decoded output file.
    */


#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"
#include "common.h"

OperationType check_operation_type(char *symbol);

int main(int argc, char *argv[])
{
    // Step 1: Check if user provided enough arguments
    if (argc < 2)
    {
        printf("Insufficient Arguments Given\n\n");
        printf("  To Encode: ./a.out -e <source_image.bmp> <secret_file> <output_stego_image.bmp>\n");
        printf("  To Decode: ./a.out -d <stego_image.bmp> <output_file>\n");
        return e_failure;
    }

    // Step 2: Check the operation type
    OperationType oprn_type = check_operation_type(argv[1]);

    // Step 3: Perform encoding
    if (oprn_type == e_encode)
    {
        //Check arguments
        if (argc < 4)
        {
            //print error
            printf("Missing arguments for encoding\n");
            printf("Give arguments like this --> ./a.out -e  source_image.bmp   secret_file   output_stego_image.bmp\n");
            return e_failure;
        }

        EncodeInfo encInfo;

        //Read and validate arguments
        if (read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            //Do encoding
            if (do_encoding(&encInfo) == e_success)
            {
                printf("Encoding completed success\n");
            }
                
            else
                printf("Error during encoding process\n");
        }
        else
        {
          
            printf("Check your file names ,try again\n");
            return e_failure;
        }
        return e_success;
    }

    // Step 4: Perform decoding
    else if (oprn_type == e_decode)
    {
        if (argc < 4)
        {
            printf("Missing arguments for decoding\n");
            printf("Give agruments like this --> ./a.out -d  stego_image.bmp   output_file\n");
            return e_failure;
        }

        DecodeInfo decInfo;

        if (read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            if (do_decoding(&decInfo) == e_success)
                printf("Decoding completed success\n");
            else
                printf("Error during decoding process\n");
        }
        else
        {
            printf("  • Stego image file is missing \n");
            printf("Check your file names and try again.\n");
        }
        return e_success;
    }

    // Step 5: Unsupported operation
    else
    {
        printf("Unsupported operation Use -e for encoding or -d for decoding\n");
        return e_failure;
    }
}

// Function to check operation type
OperationType check_operation_type(char *symbol)
{
    if (strcmp(symbol, "-e") == 0)
        return e_encode;
    else if (strcmp(symbol, "-d") == 0)
        return e_decode;
    else
        return e_unsupported;
}
