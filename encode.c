#include <stdio.h>
#include "encode.h"
#include "types.h"  // Header file containing function declarations and prototypes
#include <string.h>
#include "common.h"

/* Function Definitions */

/* 
 * Get image size for BMP
 * Input: Image file pointer
 * Output: width * height * bytes per pixel (3 in this case for RGB)
 * Description: In BMP Image, width is stored at offset 18,
 * and height follows it. The size is 4 bytes each.
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;  // Local variables to store width and height of the image
    // Seek to 18th byte (start of width in BMP header)
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (4 bytes)
    fread(&width, sizeof(int), 1, fptr_image);

    // Read the height (4 bytes)
    fread(&height, sizeof(int), 1, fptr_image);

    // Return the image size: width * height * 3 (bytes per pixel)
    return width * height * 3;
}

/* 
 * Open input and output files
 * Inputs: Source image, secret data file, and stego image file
 * Output: File pointers for above files
 * Return Value: e_success or e_failure based on file access
 */
Status open_files(EncodeInfo *encInfo)
{
    // Open the source image file for reading
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
        return e_failure;
    }

    // Open the secret data file for reading
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
        return e_failure;
    }

    // Open the stego image file for writing
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
        return e_failure;
    }

    // If no failure, return e_success
    return e_success;
}

/* 
 * Validate the encoding arguments
 * Inputs: Arguments from command line, EncodeInfo struct, and argc count
 * Output: e_success or e_failure based on argument validity
 */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo, int argc)
{
    if (argc < 3)
    {
        printf("INFO : For encoding, please pass minimum 4 arguments like ./a.out -e source_image secret_data_file\n");
        return e_failure;
    }

    // Check if source image is a BMP file
    if (strstr(argv[2], ".bmp") == NULL)
        return e_failure;

    // Check if the secret file has an extension
    if (strstr(argv[3], ".") == NULL)
        return e_failure;

    // If a custom output image name is provided, verify it is a BMP file
    if (argv[4] != NULL)
    {
        if (strstr(argv[4], ".bmp") == NULL)
        {
            return e_failure;
        }
        else
        {
             printf("INFO : Selected encoding, Encoding Started.\n\n");
             strcpy(encInfo->stego_image_fname, argv[4]);
        }
    }
    else
    {
        printf("INFO : Selected encoding, Encoding Started.\n\n");
        printf("INFO : Output file not mentioned, default file name is added\n\n");
        strcpy(encInfo->stego_image_fname, "stego.bmp");
    }

    // Set the file names in the struct
    strcpy(encInfo->src_image_fname, argv[2]);
    strcpy(encInfo->secret_fname, argv[3]);
    printf("INFO : Read and validate is Successfully executed.\n\n");
    return e_success;
}

/* Main encoding function
 * Performs all steps of encoding: opening files, checking capacity, encoding data, etc.
 */
Status do_encoding(EncodeInfo *encInfo)
{
    // Open the required files (source image, secret file, stego image)
    if (open_files(encInfo) == e_failure)
        return e_failure;
    else
        printf("INFO : Files are opened successfully.");

    // Check if the image has enough capacity to encode the secret data
    if (check_capacity(encInfo) == e_failure)
        return e_failure;
    else
        printf("\n\nINFO : Checking file capacity successfully completed.");

    // Copy BMP header to the stego image (first 54 bytes)
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("\n\nINFO : Copy BMP header is successful.");
        return e_failure;
    }

    // Encode the magic string (acting like a password for the decoder)
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_failure)
    {
        printf("Error in Encoding Magic strings");
        return e_failure;
    }
    else
        printf("\n\nINFO : Magic_string is Encoded successfully.");

    // Encode the size of the secret file extension (to be used later during decoding)
    if (encode_secret_file_extn_size(encInfo) == e_failure)
    {
        printf("\nError in Encoding secret file size extension");
        return e_failure;
    }
    else
        printf("\n\nINFO : Secret file extension size is encoded successfully.");

    // Encode the secret file extension itself
    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_failure)
    {
        return e_failure;
        printf("\nError in secret file extension");
    }
    else
        printf("\n\nINFO : Secret file extension encoded successfully.");

    // Encode the size of the secret file
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_failure)
    {
        printf("\nError in Encoding secret file size");
        return e_failure;
    }
    else
        printf("\n\nINFO : Secret file size extension encoded successfully.");

    // Encode the secret file data
    if (encode_secret_file_data(encInfo) == e_failure)
    {
        printf("\nError in encoding the secret data");
        return e_failure;
    }
    else
        printf("\n\nINFO : Secret file data is encoded successfully.");

    // Copy the remaining image data (from source image) to the stego image
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_failure)
    {
        printf("\nError in copying remaining image data");
        return e_failure;
    }
    else
        printf("\n\nINFO : Copying the remaining data is successfully completed.\n\n");

    return e_success;
}

/* 
 * Check if the image has enough capacity to hide the secret data
 * Inputs: EncodeInfo struct with details about the image and secret file
 * Output: e_success or e_failure based on available capacity
 */
Status check_capacity(EncodeInfo *encInfo)
{
    long int size;
    size = get_image_size_for_bmp(encInfo->fptr_src_image);  // Get the size of the BMP image
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);  // Get the size of the secret file
    strcpy(encInfo->extn_secret_file, strstr(encInfo->secret_fname, "."));  // Extract the extension of the secret file

    // Check if the image has enough space to store the header, magic string, file extension size,
    // file extension, file size, and secret file data
    if (size >= 54 + strlen(MAGIC_STRING) * 8 + 32 + strlen(encInfo->extn_secret_file) * 8 + 32 + (encInfo->size_secret_file) * 8)
        return e_success;
    else
        return e_failure;
}

/* Get file size */
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);  // Move to the end of the file
    return ftell(fptr);  // Return the current position (file size)
}

/* Encode the magic string into the image */
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    if (encode_data_to_image(magic_string, strlen(magic_string), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
        return e_success;
}

/* Encode data into the image (byte-by-byte) */
Status encode_data_to_image(char *data, int size, FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[8];
    for (int i = 0; i < size; i++)
    {
        fread(buffer, 8, 1, fptr_src_image);  // Read 8 bytes from the source image
        if (encode_byte_to_lsb(data[i], buffer) == e_success)  // Encode the data byte into the LSB
            fwrite(buffer, 8, 1, fptr_stego_image);  // Write the modified buffer to the stego image
    }
    return e_success;
}

/* Encode a byte of data into the Least Significant Bit (LSB) of image pixels */
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        image_buffer[i] = image_buffer[i] & ~0x01 | (((unsigned)data >> (7 - i)) & 0x01);  // Modify LSB of each byte
    }
    return e_success;
}

/* Encode the size of the secret file extension */
Status encode_secret_file_extn_size(EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer, 32, 1, encInfo->fptr_src_image);  // Read 32 bytes from source image
    if (encode_size_to_lsb(strlen(encInfo->extn_secret_file), buffer) == e_success)
        fwrite(buffer, 32, 1, encInfo->fptr_stego_image);  // Write to stego image
    return e_success;
}

/* Encode the size (in 32 bits) into the LSB of image pixels */
Status encode_size_to_lsb(int data, char *image_buffer)
{
    for (int i = 0; i < 32; i++)
    {
        image_buffer[i] = image_buffer[i] & ~0x01 | (((unsigned)data >> (31 - i)) & 0x01);  // Encode the size
    }
    return e_success;
}

/* Encode the secret file extension (like .txt, .jpg, etc.) */
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    if (encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
        return e_success;
}

/* Encode the size of the secret file */
Status encode_secret_file_size(int file_size, EncodeInfo *encInfo)
{
    char buffer[32];
    fread(buffer, 32, 1, encInfo->fptr_src_image);  // Read 32 bytes from source image
    if (encode_size_to_lsb(encInfo->size_secret_file, buffer) == e_success)
        fwrite(buffer, 32, 1, encInfo->fptr_stego_image);  // Write encoded size to stego image
    return e_success;
}

/* Encode the secret file data itself into the image */
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    char buffer[encInfo->size_secret_file];
    rewind(encInfo->fptr_secret);  // Move file pointer to the beginning of the secret file
    fread(buffer, encInfo->size_secret_file, 1, encInfo->fptr_secret);  // Read secret data

    // Encode the secret data into the image
    if (encode_data_to_image(buffer, encInfo->size_secret_file, encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        return e_success;
    }
}

/* Copy the BMP header (first 54 bytes) from source to stego image */
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char buffer[54];
    fseek(fptr_src_image, 0, SEEK_SET);  // Go to the start of the image file
    fread(buffer, 54, 1, fptr_src_image);  // Read the first 54 bytes (header)
    fwrite(buffer, 54, 1, fptr_stego_image);  // Write the header to the stego image
    return e_success;
}

/* Copy remaining image data (after the header) to the stego image */
Status copy_remaining_img_data(FILE *fptr_src_image, FILE *fptr_stego_image)
{
    char ch;
    while (fread(&ch, 1, 1, fptr_src_image) > 0)//read all bytes until EOF whose value is -1
    {
        fwrite(&ch, 1, 1, fptr_stego_image);  // Write remaining bytes to stego image
    }
    return e_success;
}
