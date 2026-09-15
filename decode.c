#include<stdio.h>
#include<string.h>
#include "types.h"
#include "decode.h"

/*
 * This file reads hidden data from a BMP image by extracting the least
 * significant bits from selected image bytes.
 * The decoder expects the same metadata layout that the encoder writes:
 * magic string size, magic string, extension size, extension, data size, data.
 */

/*
 * Validate the decode command-line arguments.
 * The input must be a BMP file. The output name is optional and defaults to output.
 */
status read_and_validate_decode_args(char *argv[],DecodeInfo *decodeinfo){

    if(strstr(argv[2],".bmp")!=NULL){
        decodeinfo->stego_file=argv[2];
    }
    else{
        printf(RED"The stego file is not a .bmp file\n"RESET);
        return e_failure;
    }

    if(argv[3]!=NULL){
        strncpy(decodeinfo->secret_fname, strtok(argv[3], "."),sizeof(decodeinfo->secret_fname) - 1);
        decodeinfo->secret_fname[sizeof(decodeinfo->secret_fname) - 1] = '\0';
    }
    else{
        strcpy(decodeinfo->secret_fname, "output");
    }
    return e_success;
}

/*
 * Open the BMP file that contains the hidden message.
 */
status open_files_for_decode(DecodeInfo *decodeinfo){

    decodeinfo->fptr_stego_image=fopen(decodeinfo->stego_file,"rb");
    if(decodeinfo->fptr_stego_image==NULL){
        perror("fopen");
        return e_failure;
    }

    return e_success;
}

/*
 * Skip the BMP header because the actual encoded content begins after 54 bytes.
 */
status skip_header_file(DecodeInfo *decodeinfo){
    fseek(decodeinfo->fptr_stego_image,54,SEEK_CUR);
    return e_success;
}

/*
 * Rebuild a 32-bit integer from 32 bytes where each byte contributes one bit.
 * This is used to decode metadata sizes.
 */
int  decode_int_from_lsb_image(char *arr){

    int get=0;
    int data_size=0;
    for(int i=0;i<32;i++){

        get=arr[i]& 1;
        data_size= data_size | (get<<i);
    }

    return data_size;
}

/*
 * Read the size of the magic string stored at the beginning of the hidden data.
 */
status decode_size_magic_string(DecodeInfo *decodeinfo){
    char arr[32];
    fread(arr,32,1,decodeinfo->fptr_stego_image);
    
    decodeinfo->size_magic_string=decode_int_from_lsb_image(arr);
    if(decodeinfo->size_magic_string <= 0 ||
       decodeinfo->size_magic_string >= 100){
        printf(RED"Invalid magic string size\n"RESET);
        return e_failure;
    }
    
    return e_success;

}

/*
 * Rebuild one character from 8 bits, where each bit is stored in the LSB of
 * one byte of the image.
 */
char decode_string_from_lsb_image(char *arr){
    int get=0;
    int data=0;
    for(int i=0;i<8;i++){

        get=arr[i]& 1;
        data= data| (get<<i);
    }

    return data;
}

/*
 * Decode the hidden magic string and compare it with the user-entered value.
 * If they do not match, the file is not valid for this program.
 */
status decode_magic_string(DecodeInfo *decodeinfo){

    char arr[8];
    char decoded_magic_string[100];
    if(decodeinfo->size_magic_string <= 0 ||
       decodeinfo->size_magic_string >= (int)sizeof(decoded_magic_string)){
        printf(RED"Invalid magic string size\n"RESET);
        return e_failure;
    }

    for(int i=0;i<decodeinfo->size_magic_string;i++){

        fread(arr,8,1,decodeinfo->fptr_stego_image);
        decoded_magic_string[i]=decode_string_from_lsb_image(arr);
        
    }
    decoded_magic_string[decodeinfo->size_magic_string]='\0';

    printf(BLUE"Enter Magic String to Varify : "RESET);
    scanf(" %99[^\n]",decodeinfo->magic_string);
    if(strcmp(decodeinfo->magic_string,decoded_magic_string)!=0){
        printf(RED"Magic String is Not Matching!\n"RESET);
        return e_failure;
    }
    
    return e_success;
}

/*
 * Read the number of characters in the secret file extension.
 */
status decode_size_of_extension(DecodeInfo *decodeinfo){

    char arr[32];
    fread(arr,32,1,decodeinfo->fptr_stego_image);
    decodeinfo->size_of_extension=decode_int_from_lsb_image(arr);
    if(decodeinfo->size_of_extension <= 0 ||
       decodeinfo->size_of_extension >= 100){
        printf(RED"Invalid extension size\n"RESET);
        return e_failure;
    }

    return e_success;

     
}

/*
 * Rebuild the stored extension such as .txt and append it to the output name.
 */
status decode_extension(DecodeInfo *decodeinfo){

    char arr[8];
    char extension[100];

    if(decodeinfo->size_of_extension <= 0 ||
       decodeinfo->size_of_extension >= (int)sizeof(extension)){
        printf(RED"Invalid extension size\n"RESET);
        return e_failure;
    }

    for(int i=0;i<decodeinfo->size_of_extension;i++){
        fread(arr,8,1,decodeinfo->fptr_stego_image);
        extension[i]=decode_string_from_lsb_image(arr);

    }
    extension[decodeinfo->size_of_extension]='\0';

    strcat(decodeinfo->secret_fname,extension);
    
    return e_success;

}

/*
 * Read the number of bytes in the secret file before extracting it.
 */
status decode_data_size(DecodeInfo *decodeinfo){

    char arr[32];
    fread(arr,32,1,decodeinfo->fptr_stego_image);
    decodeinfo->data_size=decode_int_from_lsb_image(arr);
    return e_success;

}

/*
 * Extract the hidden secret data and write it to the output file.
 */
status decode_data(DecodeInfo *decodeinfo){

    char arr[8];

    char ch;
    for(int i=0;i<decodeinfo->data_size;i++){

        fread(arr,8,1,decodeinfo->fptr_stego_image);
        ch=decode_string_from_lsb_image(arr);
        putc(ch,decodeinfo->output_file);
    }

    return e_success;
}

/*
 * Main decode flow:
 * 1. open the stego image
 * 2. skip the BMP header
 * 3. read the hidden metadata
 * 4. verify the magic string
 * 5. rebuild the extension and output file name
 * 6. extract the secret data into the output file
 */
status do_decoding(DecodeInfo *decodeinfo){

    if(open_files_for_decode(decodeinfo)!= e_success){
        printf(RED"Invalid file name\n"RESET);
        return e_failure;
    }

    skip_header_file(decodeinfo);

    if(decode_size_magic_string(decodeinfo)== e_failure){
        return e_failure;
    }

    if(decode_magic_string(decodeinfo)== e_failure){   
        return e_failure;
    }

    if(decode_size_of_extension(decodeinfo)== e_failure){
        return e_failure;
    }

    if(decode_extension(decodeinfo)== e_failure){
        return e_failure;
    }

    decodeinfo->output_file=fopen(decodeinfo->secret_fname,"wb");
    if(decodeinfo->output_file==NULL){
        perror("fopen");
        fclose(decodeinfo->fptr_stego_image);
        return e_failure;
    }

    if(decode_data_size(decodeinfo)== e_failure){
        fclose(decodeinfo->output_file);
        fclose(decodeinfo->fptr_stego_image);
        return e_failure;
    }

    decode_data(decodeinfo);   
    
    printf("The Secret Data File  is "YELLOW"\"%s\""RESET" \n",decodeinfo->secret_fname);
    printf(RED"Data is Decoded Successfuly..!"RESET"\n\n");

    fclose(decodeinfo->output_file);
    fclose(decodeinfo->fptr_stego_image);
    return e_success;
}
