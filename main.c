#include<stdio.h>
#include<string.h>
#include "types.h"
#include "encode.h"
#include "decode.h"

/*
 * Main entry point of the steganography program.
 * It checks the command line arguments and decides whether the user wants
 * to hide data in a BMP file or extract hidden data from a BMP file.
 */
int main(int argc,char *argv[]){

    operations operation;

    /* Validate the operation and its required arguments before dereferencing argv. */
    if(argc < 2){
        printf("\nPlease Use These Commands : \n");
        printf(RED"Encoding use : "BLUE"%s -e <src_file> <secret_file> <dest_file> "RESET"\n", argv[0]);
        printf(RED"Decoding use :"BLUE" %s -d <src_file> <dest_file>"RESET"\n", argv[0]);
        return e_failure;
    }

    operation = check_operation_type(argv);
    if((operation == e_encode && (argc < 4 || argc > 5)) ||
       (operation == e_decode && (argc < 3 || argc > 4)) ||
       operation == e_invalid){
        printf("\nPlease Use These Commands : \n");
        printf(RED"Encoding use : "BLUE"%s -e <src_file> <secret_file> <dest_file> "RESET"\n", argv[0]);
        printf(RED"Decoding use :"BLUE" %s -d <src_file> <dest_file>"RESET"\n", argv[0]);
        return e_failure;
    }

    /* If the operation is encoding, validate the inputs and start hiding. */
    if(operation==e_encode){
        EncodeInfo encodeinfo;
        printf(GREEN"\nYou Have Choosen Encoding.."RESET"\n");
        if(read_and_validate_encode_args(argv,&encodeinfo)==e_success){

            if (do_encoding(&encodeinfo) != e_success) {
                return e_failure;
            }
        }
        else{
            printf(RED"Invalid file name\n"RESET);
            return e_failure;
        }
    
    }
    /* If the operation is decoding, validate the inputs and start extraction. */
    else if(operation==e_decode){
        DecodeInfo decodeinfo;
        printf(GREEN"\nYou Have Choosen Decoding.."RESET"\n");
        if(read_and_validate_decode_args(argv,&decodeinfo)==e_success)
        {

            if (do_decoding(&decodeinfo) != e_success) {
                return e_failure;
            }
        }
        else{
            printf(RED"Invalid file name\n"RESET);
            return e_failure;
        }
    }
    return e_success;
}

/*
 * Reads the first command-line option and maps it to the correct operation.
 * -e means encode, -d means decode, anything else is invalid.
 */
operations check_operation_type(char *argv[]){

    if(strcmp(argv[1],"-e")==0){
        return e_encode;
    }
    else if(strcmp(argv[1],"-d")==0){
        return e_decode;
    }
    else{
        return e_invalid;
    }
}