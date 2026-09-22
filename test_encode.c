#include <stdio.h>
#include "encode.h"   // Include header for encoding operations
#include "types.h"    
#include <string.h>   
#include "decode.h"   // Include header for decoding operations

int main(int argc, char *argv[]) {
    int ret = 0, ret2 = 0;
    EncodeInfo encInfo;  // Variable for storing encoding information
    decode d;            // Variable for storing decoding information
    
    // Check the operation type from command-line arguments
    ret = check_operation_type(argc, argv); 
    
    // If encoding operation is selected
    if (ret == e_encode) {
        // Validate encoding arguments
        ret2 = read_and_validate_encode_args(argv, &encInfo, argc); 
        if (ret2 == e_success) {
            // Successfully validated encoding arguments  
            // Perform the encoding
            if (do_encoding(&encInfo) == e_success) {
                // If encoding is successful, print the success message
                printf("-----------------------------------------\n");
                printf("INFO : Encoding Successfully completed.\n");
                printf("----------------------------------------\n");
            }
        }
    }

    // If decoding operation is selected
    if (ret == e_decode) {
        // Validate decoding arguments
        if (read_and_validate_decode_args(argv, &d, argc) == e_success) {
            // Successfully validated decoding arguments
            printf("INFO : Selected Decoding, Decoding started.\n\n");
            printf("INFO : Read and validation is completed for Decoding.\n\n");

            // Perform the decoding
            if (do_decoding(&d) == e_success) {
                // If decoding is successful, print the success message
                printf("-----------------------------------------\n");
                printf("INFO : Decoding completed Successfully.\n");
                printf("-------------------------------------------\n");
            }
        }
    }

    // If no valid operation type is selected (unsupported operation)
    if (ret == e_unsupported) {
        // Provide error message on invalid arguments or operation
        printf("INFO : Please pass valid arguments.\n");
        printf("INFO : Encoding - minimum 4 arguments.\n");
        printf("INFO : Decoding - minimum 3 arguments.\n");
        return 0;
    }
}

// Function to check the operation type from command-line arguments
OperationType check_operation_type(int argc, char *argv[]) {
    // If no arguments are passed, return unsupported operation
    if (argc == 1) {
        return e_unsupported;
    }

    // Check if the operation is encoding (-e)
    if (strcmp(argv[1], "-e") == 0) {
        return e_encode;
    }

    // Check if the operation is decoding (-d)
    if (strcmp(argv[1], "-d") == 0) {
        return e_decode;
    }

    // If neither encoding nor decoding is selected, return unsupported operation
    return e_unsupported;
}

