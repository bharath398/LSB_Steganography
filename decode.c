#include <stdio.h>
#include "decode.h"   // Header file for decoding functionality
#include "types.h"    
#include <string.h>    
#include "common.h"    

// Function to validate the decoding arguments passed to the program
Status read_and_validate_decode_args(char *argv[], decode *d, int argc)
{
    if (argc < 3)
    {
        // If fewer than 3 arguments are passed, display a message and return failure
        printf("INFO : For decoding please pass minimum 3 arguments like ./a.out -d source_image\n");
        return e_failure;
    }
    if (strstr(argv[2], ".bmp") == NULL)
    {
        // If the source file is not a BMP image, return failure
        return e_failure;
    }
    if (argv[3] != NULL)
    {
        // If a secret file name is provided, copy it to the decode structure
        printf("INFO : Selected Decoding, Decoding Started.\n\n");
        strcpy(d->secret_fname, argv[3]);
    }
    else
    {
        // If no secret file name is provided, use the default name "final"
        printf("INFO : Selected Decoding, Decoding Started.\n\n");
        printf("INFO : Output file not mentioned, default file name is added\n\n");
        strcpy(d->secret_fname, "final");
    }
        strcpy(d->stego_image_fname, argv[2]);  // Store the source image file name

    return e_success;  // Return success if arguments are valid
}

// Function to open the stego image file for reading
Status open_file(decode *d)
{
    d->fptr_stego_image = fopen(d->stego_image_fname, "r");  // Open the stego image file in read mode
    if (d->fptr_stego_image == NULL)
    {
        // If the file cannot be opened, print an error and return failure
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", d->stego_image_fname);
        return e_failure;
    }
    return e_success;  // Return success if the file is opened correctly
}

// Main decoding function that coordinates the entire decoding process
Status do_decoding(decode *d)
{
    if (open_file(d) == e_failure)
    {
        printf("\nFile is not opened");
        return e_failure;  // Return failure if the file cannot be opened
    }
    else
        printf("INFO : Files are opened successfully.");

    if (decode_magic_string(MAGIC_STRING, d) == e_failure)
    {
        // If the magic string is incorrect, return failure
        printf("Error in magic string");
        return e_failure;
    }
    else
        printf("\n\nINFO : Magic string decoded successfully.");

    if (decode_secret_file_extn_size(d) == e_failure)
    {
        // If the decoding of the file extension size fails, return failure
        printf("\nError in decoding of size extension");
        return e_failure;
    }
    else
        printf("\n\nINFO : Decoding file extension size successfully.");

    if (decode_secret_file_extn(d->extn_secret_file, d) == e_failure)
    {
        return e_failure;  // Return failure if the secret file extension cannot be decoded
    }
    else
        printf("\n\nINFO : Decode file extension is completed");

    if (decode_secret_file_size(d->size_secret_file, d) == e_failure)
        return e_failure;  // Return failure if the secret file size cannot be decoded
    else
        printf("\n\nINFO : Decoded size of file is completed.");

    if (decode_secret_file_data(d) == e_failure)
        return e_failure;  // Return failure if the secret file data cannot be decoded
    else
        printf("\n\nINFO : Decoded file data is completed.\n\n");

    return e_success;  // Return success if all decoding steps complete successfully
}

// Function to decode and verify the magic string in the stego image
Status decode_magic_string(char *magic_string, decode *d)
{
    char a[strlen(magic_string) + 1];  // Buffer to store the decoded magic string
    rewind(d->fptr_stego_image);       // Rewind the file pointer to the beginning
    fseek(d->fptr_stego_image, 54, SEEK_SET);  // Skip the header of the BMP image (54 bytes)

    // Decode the magic string from the image's least significant bits
    if (decode_data_to_image(a, strlen(magic_string), d->fptr_stego_image) == e_failure)
        return e_failure;  // Return failure if decoding fails

    if (strcmp(MAGIC_STRING, a) == 0)
    {
        return e_success;  // Return success if the decoded magic string matches the expected string
    }
    return e_failure;  // Return failure if the magic string does not match
}

// Function to decode data from the stego image into a given buffer
Status decode_data_to_image(char *data, int size, FILE *fptr_stego_image)
{
    char buffer[8];  // Buffer to hold a byte's worth of data (8 bits)
    int i = 0;
    for (i = 0; i < size; i++)
    {
        fread(buffer, 8, 1, fptr_stego_image);  // Read 8 bits from the stego image
        if (decode_byte_to_lsb(&data[i], buffer) == e_failure)
        {
            return e_failure;  // Return failure if byte decoding fails
        }
    }
    data[i] = '\0';  // Null-terminate the decoded string
    return e_success;  // Return success after decoding all data
}

// Function to decode a byte from the least significant bits of the image buffer
Status decode_byte_to_lsb(char *data, char *image_buffer)
{
    *data = 0;  // Initialize the data byte to 0
    for (int i = 0; i < 8; i++)
    {
        *data = *data | ((image_buffer[i] & 0x01) << (7 - i));  // Set each bit from the image buffer
    }
    return e_success;  // Return success after decoding the byte
}

// Function to decode the secret file's extension size (32 bits) from the stego image
Status decode_secret_file_extn_size(decode *d)
{
    char buffer[32];  // Buffer to hold the extension size data (32 bits)
    int size = 0;
    fread(buffer, 32, 1, d->fptr_stego_image);  // Read 32 bits for the file extension size
    if (decode_size_to_lsb(&size, buffer) == e_success)
    {
        d->ext_size = size;  // Store the decoded extension size in the decode structure
        return e_success;
    }
    return e_failure;  // Return failure if decoding the extension size fails
}

// Function to decode a 32-bit size value from the least significant bits of the image buffer
Status decode_size_to_lsb(int *data, char *image_buffer)
{
    *data = 0;
    for (int i = 0; i < 32; i++)
    {
        *data = *data | ((image_buffer[i] & 0x01) << (31 - i));  // Set each bit from the image buffer
    }
    return e_success;  // Return success after decoding the size
}

// Function to decode the secret file extension from the stego image and create the secret file
Status decode_secret_file_extn(char *file_extn, decode *d)
{
    if (decode_data_to_image(file_extn, d->ext_size, d->fptr_stego_image) == e_success)
    {
        strcat(d->secret_fname, file_extn);  // Append the file extension to the secret file name
        d->fptr_secret = fopen(d->secret_fname, "w");  // Open the secret file for writing
        if (d->fptr_secret == NULL)
        {
            perror("fopen");
            fprintf(stderr, "ERROR: Unable to open file %s\n", d->secret_fname);
            return e_failure;  // Return failure if the secret file cannot be opened
        }
        return e_success;
    }
    return e_failure;  // Return failure if decoding the extension fails
}

// Function to decode the secret file size (32 bits) from the stego image
Status decode_secret_file_size(int size, decode *d)
{
    char buffer[32];  // Buffer to hold the file size data (32 bits)
    fread(buffer, 32, 1, d->fptr_stego_image);  // Read 32 bits for the file size
    if (decode_size_to_lsb(&size, buffer) == e_success)
    {
        d->size_secret_file = size;  // Store the decoded file size in the decode structure
        return e_success;
    }
    return e_failure;  // Return failure if decoding the file size fails
}

// Function to decode the secret file data from the stego image and write it to the secret file
Status decode_secret_file_data(decode *d)
{
    char buffer[(d->size_secret_file) + 1];  // Buffer to hold the decoded file data
    if (decode_data_to_image(buffer, d->size_secret_file, d->fptr_stego_image) == e_success)
    {
        fprintf(d->fptr_secret, "%s", buffer);  // Write the decoded data to the secret file
        return e_success;
    }
    return e_failure;  // Return failure if decoding the file data fails
}
