#ifndef DECODE_H
#define DECODE_H
#include "types.h"  



// Structure to hold the information for decoding the stego image
typedef struct _decode
{
    /* Secret File Info */
    char secret_fname[100];      // Name of the secret file to save decoded data
    FILE *fptr_secret;          // File pointer for the secret file
    char extn_secret_file[10];  // File extension of the secret file
    char secret_data[100];  // Buffer for decoded secret data
    int size_secret_file;       // Size of the secret file
    int ext_size;               // Size of the file extension for decoding

    /* Stego Image Info */
    char stego_image_fname[100]; // Name of the stego image file (encoded image)
    FILE *fptr_stego_image;     // File pointer for the stego image

} decode;  

/* Function Declarations */

/* Validate the decoding arguments (check if the passed arguments are correct)*/
Status read_and_validate_decode_args(char *argv[], decode *d, int argc);

/* Open the necessary files: stego image and secret file*/
Status open_file(decode *d);

/* Perform the decoding operation, handling all the necessary steps*/
Status do_decoding(decode *d);

/*  Decode the magic string (a special string used to identify the encoding)*/
Status decode_magic_string(char *magic_string, decode *d);

/* Extract data from the stego image, converting image pixels back to data bits*/
Status decode_data_to_image(char *data, int size, FILE *fptr_stego_image);

/*  Extract a byte of data from the least significant bits of the image buffer*/
Status decode_byte_to_lsb(char *data, char *image_buffer);

/* Decode the size of the secret file extension from the stego image*/
Status decode_secret_file_extn_size(decode *d);

/* Extract a size value from the least significant bits of the image buffer*/
Status decode_size_to_lsb(int *data, char *image_buffer);

/* Decode the secret file's extension (e.g., ".txt", ".jpg") from the stego image*/
Status decode_secret_file_extn(char *file_extn, decode *d);

/*  Decode the size of the secret file from the stego image*/
Status decode_secret_file_size(int file_size, decode *d);

/* Decode the secret file data from the stego image and save it*/
Status decode_secret_file_data(decode *d);

#endif 
