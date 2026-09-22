#ifndef ENCODE_H
#define ENCODE_H
#include "types.h" 

/* 
 * Structure to store information required for
 * encoding the secret file into the source Image.
 * Information about output and intermediate data is also stored.
 */

#define MAX_SECRET_BUF_SIZE 1              // Maximum buffer size for secret data (in bytes)
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)  // Maximum buffer size for image data
#define MAX_FILE_SUFFIX 4                  // Maximum length for file extension (e.g., ".txt")

/* EncodeInfo structure holds all the necessary data for encoding */
typedef struct _EncodeInfo
{
    /* Source Image info */
    char src_image_fname[100];           // Filename of the source image
    FILE *fptr_src_image;               // File pointer for the source image
    uint image_capacity;                 // Capacity of the image (in bytes)
    uint bits_per_pixel;                 // Bits per pixel of the image
    char image_data[MAX_IMAGE_BUF_SIZE]; // Image data buffer

    /* Secret File Info */
    char secret_fname[100];             // Filename of the secret file
    FILE *fptr_secret;                  // File pointer for the secret file
    char extn_secret_file[MAX_FILE_SUFFIX];  // File extension of the secret file
    char secret_data[MAX_SECRET_BUF_SIZE];  // Secret data buffer
    int size_secret_file;               // Size of the secret file

    /* Stego Image Info */
    char stego_image_fname[100];        // Filename for the stego image
    FILE *fptr_stego_image;             // File pointer for the stego image

} EncodeInfo;  // End of EncodeInfo structure

/* Function Prototypes */

/* Check the operation type based on command-line arguments */
OperationType check_operation_type(int argc, char *argv[]);

/* Read and validate the encoding arguments */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo, int argc);

/* Perform the encoding process */
Status do_encoding(EncodeInfo *encInfo);

/* Open files required for encoding */
Status open_files(EncodeInfo *encInfo);

/* Check if the source image has enough capacity to store the secret file */
Status check_capacity(EncodeInfo *encInfo);

/* Get the size of the image (used for BMP files) */
uint get_image_size_for_bmp(FILE *fptr_image);

/* Get the size of the given file */
uint get_file_size(FILE *fptr);

/* Copy the BMP header from the source image to the stego image */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

/* Encode the magic string into the image to identify the stego image */
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo);

/* Encode the secret file extension size into the image */
Status encode_secret_file_extn_size(EncodeInfo *encInfo);

/* Encode an integer value (e.g., secret file size) into the least significant bit (LSB) of image data */
Status encode_size_to_lsb(int data, char *image_buffer);

/* Encode the secret file extension into the image */
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo);

/* Encode the secret file size into the image */
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo);

/* Encode the secret file data into the image */
Status encode_secret_file_data(EncodeInfo *encInfo);

/* The actual encoding process that embeds data into the image */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image);

/* Encode a byte of data into the least significant bit of the image data buffer */
Status encode_byte_to_lsb(char data, char *image_buffer);

/* Copy the remaining image data from the source image to the stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest);

#endif
