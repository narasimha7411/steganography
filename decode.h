#ifndef DECODE_H
#define DECODE_H

#include "types.h"

typedef struct _DecodeInfo{
    
    char *stego_file;
    FILE *fptr_stego_image;

    int size_magic_string;
    char magic_string[100];

    int size_of_extension;
    char secret_fname[100];

    FILE *output_file;
    int data_size;



}DecodeInfo;

status read_and_validate_decode_args(char *argv[],DecodeInfo *decodeinfo);

status open_files_for_decode(DecodeInfo *decodeinfo);

status do_decoding(DecodeInfo *decodeinfo);

status skip_header_file(DecodeInfo *decodeinfo);

status decode_size_magic_string(DecodeInfo *decodeinfo);

int decode_int_from_lsb_image(char *arr);

char decode_string_from_lsb_image(char *arr);

status decode_magic_string(DecodeInfo *decodeinfo);

status decode_size_of_extension(DecodeInfo *decodeinfo);

status decode_extension(DecodeInfo *decodeinfo);

status decode_data_size(DecodeInfo *decodeinfo);

status decode_data(DecodeInfo *decodeinfo);


#endif