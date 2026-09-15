#ifndef ENCODE_H
#define ENCODE_H
#include "types.h"

typedef struct _EncodeInfo{
    char *src_file;
    FILE *fptr_src_image;
    uint src_image_size;

    char *secret_file;
    FILE *fptr_secret_message;
    uint secret_file_size;
    char secret_file_extension[5];

    char *stego_file;
    FILE *fptr_stego_image;

    char magic_string[100];
}EncodeInfo;


operations check_operation_type(char *argv[]);

status validate_secret_file_type(char *argv[],EncodeInfo *encodeinfo);

status read_and_validate_encode_args(char *argv[],EncodeInfo *encodeinfo);

status open_files_for_encode(EncodeInfo *encodeinfo);

status check_capacity(EncodeInfo *encodeinfo);

status do_encoding(EncodeInfo *encodeinfo);

status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image);

void encode_int_to_lsb_image(char *arr,int data);

void encode_string_to_lsb_image(char arr[],int data);

status encode_magic_string_size(EncodeInfo *encodeinfo);

status encode_magic_string(EncodeInfo *encodeinfo);

status encode_secret_file_extension_size(EncodeInfo *encodeinfo);

status encode_secret_file_extension(EncodeInfo *encodeinfo);

status encode_secret_data_size(EncodeInfo *encodeinfo);

status encode_secret_data(EncodeInfo *encodeinfo);


#endif