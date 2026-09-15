#include<stdio.h>
#include<string.h>
#include "types.h"
#include "encode.h"

/*
 * This file is responsible for hiding a secret file inside a BMP image.
 * The idea is simple: the program reads the image and modifies only the least
 * significant bit (LSB) of selected bytes so the image remains visually almost
 * unchanged while hidden data is stored inside it.
 *
 * The hidden data contains:
 *  1. magic string size
 *  2. magic string
 *  3. secret file extension size
 *  4. secret file extension
 *  5. secret file size
 *  6. actual secret data
 */

// Supported secret file extensions are .txt, .mp3, .pdf, .jpg, .png, and .mp4.
status validate_secret_file_type(char *argv[],EncodeInfo *encodeinfo){


    char *txt = strrchr(argv[3],'.');

    if(txt == NULL ){
        printf(RED"The secret file type is not supported..!\n"RESET);
        return e_failure;
    }

    if(strcmp(txt,".txt")==0){
        encodeinfo->secret_file=argv[3];
        strcpy(encodeinfo->secret_file_extension,".txt");
        return e_success;
    }
    
    if(strcmp(txt,".mp3")==0){
        encodeinfo->secret_file=argv[3];
        strcpy(encodeinfo->secret_file_extension,".mp3");
        return e_success;
    }
    
    if(strcmp(txt,".pdf")==0){
        encodeinfo->secret_file=argv[3];
        strcpy(encodeinfo->secret_file_extension,".pdf");
        return e_success;
    }

    if(strcmp(txt,".jpg")==0){
        encodeinfo->secret_file=argv[3];
        strcpy(encodeinfo->secret_file_extension,".jpg");
        return e_success;
    }
    if(strcmp(txt,".png")==0){
        encodeinfo->secret_file=argv[3];
        strcpy(encodeinfo->secret_file_extension,".png");
        return e_success;
    }
    if(strcmp(txt,".mp4")==0){
        encodeinfo->secret_file=argv[3];
        strcpy(encodeinfo->secret_file_extension,".mp4");
        return e_success;
    }

    return  e_failure;
}
 
/*
 * Validate the command line arguments for encoding.
 * The source file should be a BMP image, the secret file should use a supported extension,
 * and the destination file should also be a BMP if the user provides one.
 */
status read_and_validate_encode_args(char *argv[],EncodeInfo *encodeinfo){
    if(strstr(argv[2],".bmp")!=NULL){

        encodeinfo->src_file=argv[2];
    }
    else{
        printf(RED"The srource file is not a .bmp file\n"RESET);
        return e_failure;
    }

    if(validate_secret_file_type(argv,encodeinfo)!=e_success){
        return e_failure;
    } 
    
    if(argv[4]==NULL){
        encodeinfo->stego_file="stego.bmp";

    }
    else{
        if(strstr(argv[4],".bmp")!=NULL){
            encodeinfo->stego_file=argv[4];
        }
        else{
            printf(RED"The stego file is not a .bmp file\n"RESET);
            return e_failure;
        }
    }

    return e_success;
}

/*
 * Open all required files for encoding.
 * The source image is read, the secret file is read, and the output stego image
 * is created in write mode.
 */
status open_files_for_encode(EncodeInfo *encodeinfo){

    encodeinfo->fptr_src_image=fopen(encodeinfo->src_file,"rb");
    if(encodeinfo->fptr_src_image==NULL){
        perror("fopen");
        return e_failure;
    }

    encodeinfo->fptr_secret_message=fopen(encodeinfo->secret_file,"rb");
    if(encodeinfo->fptr_secret_message==NULL){
        perror("fopen");
        return e_failure; 
    }

    encodeinfo->fptr_stego_image=fopen(encodeinfo->stego_file,"wb");
    if(encodeinfo->fptr_stego_image==NULL){
        perror("fopen");
        return e_failure;
    }

    return e_success;
}

/*
 * Read the BMP image dimension from the header.
 * The width and height are needed to estimate whether the image has enough
 * space to store the hidden message.
 */
uint get_file_size(FILE *fptr){
   uint width,length;
   fseek(fptr,18,SEEK_SET);

   fread(&width,4,1,fptr);
   //printf("Width of image is %u\n",width);


   fread(&length,4,1,fptr);
   //printf("Length of image is %u\n",length);

   rewind(fptr);
   return width*length*3;
}

/*
 * Check whether the secret data will fit into the image.
 * We calculate the total hidden payload size and compare it with the image size.
 */
status check_capacity(EncodeInfo *encodeinfo){

    fseek(encodeinfo->fptr_secret_message,0,SEEK_END);

    encodeinfo->secret_file_size =ftell(encodeinfo->fptr_secret_message);
    size_t magic_len = strlen(encodeinfo->magic_string);
    size_t ext_len = strlen(encodeinfo->secret_file_extension);
    size_t total_bits = 32 + (magic_len * 8) +
                       32 + (ext_len * 8) +
                       32 + (encodeinfo->secret_file_size * 8);

    encodeinfo->src_image_size =get_file_size(encodeinfo->fptr_src_image);

    rewind(encodeinfo->fptr_secret_message);

    if(total_bits > (encodeinfo->src_image_size * 8)){
        printf(RED"The source image is not sufficient to encode the secret file\n"RESET);
        return e_failure;
    }
    
    return e_success;
}

/*
 * Copy the 54-byte BMP header into the output image.
 * This keeps the new image valid and recognizable as a BMP file.
 */
status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image){

    char str[54];
    fread(str,54,1,fptr_src_image);
    fwrite(str,54,1,fptr_dest_image);

    return e_success;
}

/*
 * Store an integer value into 32 bytes by changing only the least significant
 * bit of each byte. This is used for sizes such as the magic string length.
 */
void encode_int_to_lsb_image(char *arr,int data){
    for(int i=0;i<32;i++){

        arr[i]=arr[i] &(~1);
        int get_bit=(data&(1<<i))>>i;
        arr[i]=arr[i] | get_bit;
    }

}
/*
 * Write the size of the magic string before the actual string.
 */
status encode_magic_string_size(EncodeInfo *encodeinfo){
    int size=strlen(encodeinfo->magic_string);
    char arr[32];
    fread(arr,32,1,encodeinfo->fptr_src_image);
    encode_int_to_lsb_image(arr,size);
    fwrite(arr,32,1,encodeinfo->fptr_stego_image);
    
    return e_success;
}

/*
 * Store a single character into 8 bytes by modifying their LSB values.
 */
void encode_string_to_lsb_image(char *arr,int data){

   for(int i=0;i<8;i++){

        arr[i]=arr[i] &(~1);
        int get_bit=(data&(1<<i))>>i;
        arr[i]=arr[i] | get_bit;
    }
}

/*
 * Embed the actual magic string character by character.
 */
status encode_magic_string(EncodeInfo *encodeinfo){
    int size=strlen(encodeinfo->magic_string);
    char arr[8];
    for(int i=0;i<size;i++){
        
        fread(arr,8,1,encodeinfo->fptr_src_image);
        encode_string_to_lsb_image(arr,encodeinfo->magic_string[i]);
        fwrite(arr,8,1,encodeinfo->fptr_stego_image);
    }
    
    
    return e_success;
}

/*
 * Save the number of characters in the secret file extension.
 */
status encode_secret_file_extension_size(EncodeInfo *encodeinfo){

    int size= strlen(encodeinfo->secret_file_extension);

    char arr[32];
    fread(arr,32,1,encodeinfo->fptr_src_image);
    encode_int_to_lsb_image(arr,size);
    fwrite(arr,32,1,encodeinfo->fptr_stego_image);

    return e_success;

}

/*
 * Encode the file extension, such as .txt, so the decoder knows how to name
 * the extracted output file.
 */
status encode_secret_file_extension(EncodeInfo *encodeinfo){

    int size=strlen(encodeinfo->secret_file_extension);
    char arr[8];
    for(int i=0;i<size;i++){
       
        fread(arr,8,1,encodeinfo->fptr_src_image);
        encode_string_to_lsb_image(arr,encodeinfo->secret_file_extension[i]);
        fwrite(arr,8,1,encodeinfo->fptr_stego_image);

    }

    return e_success;
}
/*
 * Store the length of the secret data before writing the actual bytes.
 */
status encode_secret_data_size(EncodeInfo *encodeinfo){

    int size = encodeinfo->secret_file_size;

    char arr[32];

    fread(arr,32,1,encodeinfo->fptr_src_image);
    encode_int_to_lsb_image(arr,size);
    fwrite(arr,32,1,encodeinfo->fptr_stego_image);

    return e_success;
}

/*
 * Hide the real content of the secret file, one byte at a time.
 */
status encode_secret_data(EncodeInfo *encodeinfo){

    int size=encodeinfo->secret_file_size;
    char arr[8];
    int ch;
    for(int i=0;i<size;i++){

        ch=fgetc(encodeinfo->fptr_secret_message);

        if(ch==EOF){
            printf(RED"Failed to read secret file completly\n"RESET);
            return e_failure;
        }
        fread(arr,8,1,encodeinfo->fptr_src_image);
        encode_string_to_lsb_image(arr,ch);
        fwrite(arr,8,1,encodeinfo->fptr_stego_image);
    }
    return e_success;
}

/*
 * Main encoding flow:
 * 1. open all files
 * 2. read the magic string from the user
 * 3. verify capacity
 * 4. copy the BMP header
 * 5. write metadata and secret data
 * 6. copy remaining bytes from the source image
 */
status do_encoding(EncodeInfo *encodeinfo){

    if(open_files_for_encode(encodeinfo)!=e_success){
        printf(RED"Error in opening files\n"RESET);
        return e_failure;
    }
    printf(BLUE "Enter the Magic String : "RESET);
    scanf(" %99[^\n]",encodeinfo->magic_string);

    if(check_capacity(encodeinfo)==e_failure){
        return e_failure;
    }

    if(copy_bmp_header(encodeinfo->fptr_src_image, encodeinfo->fptr_stego_image)==e_failure){
        printf(RED"Error in copying header\n"RESET);
        return e_failure;
    }
    
    encode_magic_string_size(encodeinfo);
    encode_magic_string(encodeinfo);

    encode_secret_file_extension_size(encodeinfo);
    encode_secret_file_extension(encodeinfo);

    encode_secret_data_size(encodeinfo);
    encode_secret_data(encodeinfo);

    int  ch;
    while((ch=getc(encodeinfo->fptr_src_image))!=EOF){

        putc(ch,encodeinfo->fptr_stego_image);
    }

    printf("The Output File  is "YELLOW"\"%s\""RESET" \n",encodeinfo->stego_file);
    printf(RED"Data is Encoded Successfuly..!"RESET"\n\n");
    fclose(encodeinfo->fptr_src_image);
    fclose(encodeinfo->fptr_secret_message);
    fclose(encodeinfo->fptr_stego_image);
    return e_success;
}