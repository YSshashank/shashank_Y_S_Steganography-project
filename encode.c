#include "encode.h"
#include "types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

uint get_file_size(FILE *fptr)
{
    // Find the size of secret file data
    fseek(fptr, 0, SEEK_END);
    uint size = ftell(fptr);
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

EncodeStatus read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // Step 1 : check argv[2] is having .bmp extension or not
    encInfo->src_image_fname = argv[2];
    char *dot = strrchr(encInfo->src_image_fname, '.'); // strstr() is not safe here
    if (dot == NULL || strcmp(dot, ".bmp") != 0)
    {
        // False return e_failure
        printf("Missing source file\n");
        return e_failure;
    }
    else
    {
        // True store in structure
        encInfo->src_image_fname = argv[2];
    }

    // Step 2 : check argv[3] is having .txt extension or not
    encInfo->secret_fname = argv[3];
    dot = strrchr(encInfo->secret_fname, '.'); // strstr() is not safe here
    if (dot == NULL || strcmp(dot, ".txt") != 0)
    {
        // False return e_failure
        printf("Missing Secret file\n");
        return e_failure;
    }
    else
    {
        // True store in structure
        encInfo->secret_fname = argv[3];
    }

    // Step 3 : check argv[4] is having NULL or not
    if (argv[4] != NULL) 
    {
        encInfo->stego_image_fname = argv[4];
        dot = strrchr(encInfo->stego_image_fname, '.'); // strstr() is not safe here
        if (dot == NULL || strcmp(dot, ".bmp") != 0)
        {
            // False return e_failure
            printf("Missing dest.bmp\n");
            return e_failure;
        }
    }
    else
    {
        // True store default name (default.bmp)into structure member
        encInfo->stego_image_fname = "encoded.bmp";
    }

    return e_success;
}


//Step 1 : open encode files
EncodeStatus open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        return e_failure;
    }

    // No failure return e_success
    return e_success;
}


//Step 2 : check the capacity
EncodeStatus check_capacity(EncodeInfo *encInfo)
{
    // call and check get_image_size_for_bmp(encInfo->fptr_src_image)
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    // store into structure member

    // call and check get_file_size(encInfo->fptr_secret)
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    // store into structure member

    // check image_capacity > (8*(MAGIC_STRING) + 32(sizeof(file extn)) + 32(sizeof(extn)) + 32(sizeof(secret file)) +8 *(sizeof(file size)) ))
    if (encInfo->image_capacity > (8 * strlen(MAGIC_STRING) + 32 + 32 + 32 + 8 * encInfo->size_secret_file))
    {
        // True return e_success
        return e_success;
    }
    else
    {
        // False return e_failure
        return e_failure;
    }
}


//Step 3: copy bmp image header
EncodeStatus copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    //step 1 : Rewind the src_file_fptr
    fseek(fptr_src_image, 0, SEEK_SET);  
    rewind(fptr_src_image);

    //step 2 : Declare char imageBuffer[54]
    char imageBuffer[54];

    //step 3 : fread 54 bytes from src_image file and store into imageBuffer
    fread(imageBuffer, sizeof(char), 54, fptr_src_image);

    //step 4 : fwrite the 54 bytes into dest_image file from imageBuffer
    fwrite(imageBuffer, sizeof(char), 54, fptr_dest_image);

    //step 5 : check the both fptr offset pointing to the same offset or not
    if (ftell(fptr_src_image) == ftell(fptr_dest_image))
    {
        //true return e_success
        return e_success;
    }
    else
    {
        //false return e_failure
        return e_failure;
    }
    
}


//Step 4 : Encode Magic String
EncodeStatus encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
    //Step 1 : char imageBuffer[8];
    char imageBuffer[8];

    //step 2 : Generate the loop upto the length of magic string
    for (int i = 0; i < strlen(magic_string); i++)
    {
        //step 3 : Read 8 bytes from src image and store into imageBuffer
        fread(imageBuffer, sizeof(char), 8, encInfo->fptr_src_image);

        //step 4 : call the encode_byte_to_lsb(magic_string[i], *imageBuffer)
        encode_byte_to_lsb(magic_string[i], imageBuffer);

        //step 5 : write the 8 bytes from imageBuffer to stego image
        fwrite(imageBuffer, sizeof(char), 8, encInfo->fptr_stego_image);
    }

    //step 6 : check the both fptr offset pointing to the same offset or not
    if (ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_stego_image))
    {
        //true return e_success
        return e_success;
    }
    else
    {
        //false return e_failure
        return e_failure;
    }
     
}


//Step 5 : encode_secret_file_extn_size
EncodeStatus encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    //Step 1 : char imageBuffer[32];
    char imageBuffer[32];
    
    //step 2 : Read 32 bytes from src image and store into imageBuffer
    fread(imageBuffer, sizeof(char), 32, encInfo->fptr_src_image);

    //step 3 : call the encode_byte_to_lsb(size, *imageBuffer)
    encode_size_to_lsb(size, imageBuffer);

    //step 4 : write the imageBuffer to stego image
    fwrite(imageBuffer, sizeof(char), 32, encInfo->fptr_stego_image);

    //step 5 : check the both fptr offset pointing to the same offset or not
    if (ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_stego_image))
    {
        //true return e_success
        return e_success;
    }
    else
    {
        //false return e_failure
        return e_failure;
    }
}


//Step 6 : encode_secret_file_extn
EncodeStatus encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
    //Step 1 : char imageBuffer[8];
    char imageBuffer[8];

    //step 2 : Generate the loop upto the length of file_extn
    for (int i = 0; i < strlen(file_extn); i++)
    {
        //step 3 : Read 8 bytes from src image and store into imageBuffer
         fread(imageBuffer, sizeof(char), 8, encInfo->fptr_src_image);

        //step 4 : call the encode_byte_to_lsb(file_extn[i], *imageBuffer)
        encode_byte_to_lsb(file_extn[i], imageBuffer);

        //step 5 : write the 8 bytes from imageBuffer to stego image
        fwrite(imageBuffer, sizeof(char), 8, encInfo->fptr_stego_image);
    }

    //step 6 : check the both fptr offset pointing to the same offset or not
    if (ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_stego_image))
    {
        //true return e_success
        return e_success;
    }
    else
    {
        //false return e_failure
        return e_failure;
    }
}


//Step 7 : encode_secret_file_size
EncodeStatus encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    //Step 1 : char imageBuffer[32];
    char imageBuffer[32];
    
    //step 2 : Read 32 bytes from src image and store into imageBuffer
    fread(imageBuffer, sizeof(char), 32, encInfo->fptr_src_image);

    //step 3 : call the encode_byte_to_lsb(file_size, *imageBuffer)
    encode_size_to_lsb(file_size, imageBuffer);

    //step 4 : write the imageBuffer to stego image
    fwrite(imageBuffer, sizeof(char), 32, encInfo->fptr_stego_image);

    //step 5 : check the both fptr offset pointing to the same offset or not
    if (ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_stego_image))
    {
        //true return e_success
        return e_success;
    }
    else
    {
        //false return e_failure
        return e_failure;
    }
}


//Step 8 : encode_secret_file_data
EncodeStatus encode_secret_file_data(EncodeInfo *encInfo)
{
    char imageBuffer[8];
    char data;

    // Rewind to start of secret file
    rewind(encInfo->fptr_secret);

    for (long i = 0; i < encInfo->size_secret_file; i++)
    {
        // Read one byte from secret file
        fread(&data, sizeof(char), 1, encInfo->fptr_secret);

        // Read 8 bytes from source image
        fread(imageBuffer, sizeof(char), 8, encInfo->fptr_src_image);

        // Encode this byte into the image bytes
        encode_byte_to_lsb(data, imageBuffer);

        // Write encoded bytes to stego image
        fwrite(imageBuffer, sizeof(char), 8, encInfo->fptr_stego_image);
    }

    //step 10 : check the both fptr offset pointing to the same offset or not
    if (ftell(encInfo->fptr_src_image) == ftell(encInfo->fptr_stego_image))
    {
        //true return e_success
        return e_success;
    }
    else
    {
        //false return e_failure
        return e_failure;
    }
}


//Step 9 : copy remaining image bytes from src to stego image
EncodeStatus copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    //Declare variable
    char ch;

    //Read byte by byte from src and write to dest until EOF
    while (fread(&ch, sizeof(char), 1, fptr_src) > 0)
    {
        fwrite(&ch, sizeof(char), 1, fptr_dest);
    }
    return e_success;
}


//Step 10 :Encode one byte (character) into 8 bytes of image data
EncodeStatus encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data >> i) & 1);
    }
    return e_success;
}

//Step 11 : encode a size to lsb
EncodeStatus encode_size_to_lsb(int size, char *imageBuffer)
{
    //Generate the loops for 32 times
    for (int i = 0; i < 32; i++)
    {
        //step 1 : clear the LSB of image buffer[i]
        imageBuffer[i] &= 0xFE;
        //step 2 : Take the bit from data

        //extracting lsb bit first
        char bit = (size >> i) & 0x01;
        
        //step 3 : set the bit into the lsb of image buffer
        imageBuffer[i] |= bit;
    }
    return e_success;
}


//Step 12 : do_encoding
EncodeStatus do_encoding(EncodeInfo *encInfo)
{
    // step 1 : call the open_files(encInfo) == e_success
    if (open_files(encInfo) == e_success)
    {
        // True print the prompt message
        printf("All files opened success\n");
    }
    else
    {
        // False return e_failure
        return e_failure;
    }


    // Step 2 : check the capacity
    if (check_capacity(encInfo) == e_success)
    {
        // True print the prompt message
        printf("Image has enough capacity to hold secret data\n");
    }
    else
    {
        // False print the prompt message
        return e_failure;
    }


    //step 3: call the bmp heder copy_bmp_header(fptr_src_image, fptr_stego_image) == e_success
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        //true print the prompt message
        printf("BMP Header copied success\n");
    }
    else
    {
        //false return e_failure 
        return e_failure;
    }
    
    

    // step 4 : Encode Magic String(MAGIC_STRING, encInfo) == e_success
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
    {
        // true print the prompt message
        printf("Magic string encoded  \n");
    }
    else
    {
        // false return e_failure
        return e_failure;
    }
    
    
    //step 5 : store the secret file extntion into  extn_secret_file(.txt)
    strcpy(encInfo->extn_secret_file, ".txt");
    
    //step 6 : Find the size of the extn_secret_file and move into one variable
    int size = strlen(encInfo->extn_secret_file);


    //step 7 : encode_secret_file_extn_size(size, encInfo) == e_success
    if (encode_secret_file_extn_size(size, encInfo) == e_success)
    {
        // true print the prompt message
        printf("Secret file extension size encoded : %d\n",size);
    }
    else
    {
        // false return e_failure
        return e_failure;
    }


    // step 8 : encode_secret_file_extn(extn_secret_file, encInfo) == e_success
    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
    {
        // true print the prompt message
        printf("Secret file extension encoded Success\n");
    }
    else
    {
        // false return e_failure
        return e_failure;
    }   
    // step 9 : encode_secret_file_size(size_secret_file, encInfo) == e_success
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
    {
        // true print the prompt message
        printf("Secret file size encoded : %ld\n",encInfo->size_secret_file);         
    }
    else
    {
        // false return e_failure
        return e_failure;
    }


    // step 10 : encode_secret_file_data(encInfo) == e_success
    if (encode_secret_file_data(encInfo) == e_success)
    {
        // true print the prompt message
        printf("Secret file data encoded success\n");
    }
    else
    {
        // false return e_failure
        return e_failure;
    }


    // step 11 : copy_remaining_img_data(fptr_src_image, fptr_stego_image) == e_success
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        // true print the prompt message
        printf("Remaining image data copied success\n");
        return e_success;
    }
    else
    {
        // false return e_failure
        return e_failure;
    }
    return e_success;
}
