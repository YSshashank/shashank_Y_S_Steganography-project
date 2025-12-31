#ifndef ENCODE_H
#define ENCODE_H
#include <stdio.h>

#include "types.h" // Contains user defined types

/*
 * Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

typedef struct _EncodeInfo
{
    /* Source Image info */
    char *src_image_fname; // To store the src image name
    FILE *fptr_src_image;  // To store the address of the src image
    uint image_capacity;   // To store the size of image

    /* Secret File Info */
    char *secret_fname;       // To store the secret file name
    FILE *fptr_secret;        // To store the secret file address
    char extn_secret_file[5]; // To store the Secret file extension
    char secret_data[100];    // To store the secret data
    long size_secret_file;    // To store the size of the secret data

    /* Stego Image Info */
    char *stego_image_fname; // To store the dest file name
    FILE *fptr_stego_image;  // To store the address of stego image

} EncodeInfo;

/* Encoding function prototype */

/* Read and validate Encode args from argv */
EncodeStatus read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo);

/* Perform the encoding */
EncodeStatus do_encoding(EncodeInfo *encInfo);

/* Get File pointers for i/p and o/p files */
EncodeStatus open_files(EncodeInfo *encInfo);

/* check capacity */
EncodeStatus check_capacity(EncodeInfo *encInfo);

/* Get image size */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get file size */
uint get_file_size(FILE *fptr);

/* Copy bmp image header */
EncodeStatus copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Store Magic String */
EncodeStatus encode_magic_string(const char *magic_string, EncodeInfo *encInfo);

/*Encode extension size*/
EncodeStatus encode_secret_file_extn_size(int size, EncodeInfo *encInfo);

/* Encode secret file extenstion */
EncodeStatus encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo);

/* Encode secret file size */
EncodeStatus encode_secret_file_size(long file_size, EncodeInfo *encInfo);

/* Encode secret file data*/
EncodeStatus encode_secret_file_data(EncodeInfo *encInfo);

/* Encode a byte into LSB of image data array */
EncodeStatus encode_byte_to_lsb(char data, char *image_buffer);

// Encode a size to lsb
EncodeStatus encode_size_to_lsb(int size, char *imageBuffer);

/* Copy remaining image bytes from src to stego image after encoding */
EncodeStatus copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif
