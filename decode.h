#ifndef DECODE_H
#define DECODE_H
#include <stdio.h>

#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * decoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */
 
typedef struct _DecodeInfo
{
    /* Source Image info */
    char *src_image_fname; // To store the src image name
    FILE *fptr_src_image;  // To store the address of the src image
    uint image_capacity;   // To store the size of image

    /* Secret File Info */
    char *secret_fname;       // To store the secret file name
    FILE *fptr_secret;        // To store the secret file address
    char extn_secret_file[10]; // To store the Secret file extension
    char secret_data[100];    // To store the secret data
    long size_secret_file;    // To store the size of the secret data

    /* Stego Image Info */
    char *stego_image_fname; // To store the dest file name
    FILE *fptr_stego_image;  // To store the address of stego image

} DecodeInfo;

/* Decoding function prototype */

/* Read and validate Decode args from argv */
DecodeStatus read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
DecodeStatus do_decoding(DecodeInfo *decInfo);

/* Get File pointers for i/p and o/p files */
DecodeStatus open_decode_files(DecodeInfo *decInfo);


/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);


/* Store Magic String */
DecodeStatus decode_magic_string(const char *magic_string, DecodeInfo *decInfo);

/* Decode extension size */
DecodeStatus decode_secret_file_extn_size(int *size, DecodeInfo *decInfo);

/* Decode secret file extension */
DecodeStatus (decode_secret_file_extn(char *file_extn, int extn_size, DecodeInfo *decInfo));

/* Decode secret file size */
DecodeStatus decode_secret_file_size(int *file_size, DecodeInfo *decInfo);

/* Decode secret file data*/
DecodeStatus decode_secret_file_data(DecodeInfo *decInfo, int file_size);

/* Decode a byte into LSB of image data array */
DecodeStatus decode_byte_from_lsb(char *data, char *image_buffer);

// Decode a size to lsb
DecodeStatus decode_size_from_lsb(int *size, char *imageBuffer);





#endif
