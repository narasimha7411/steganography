# Image Steganography using C

A command-line tool that hides a file inside a BMP image using Least
Significant Bit (LSB) steganography, and extracts it back out with no data
loss. Implemented from scratch in C with no external libraries.

## Technologies Used

- C Programming
- File Handling
- Bitwise Operations
- BMP Image Processing
- Structures and Pointers
- Command Line Arguments

## Description

Image Steganography is a technique used to hide secret information inside
an image without visibly affecting the image quality. This project
implements image steganography using the Least Significant Bit (LSB)
technique in C.

The application hides secret files such as `.txt`, `.jpg`, `.png`, `.pdf`,
`.mp3`, and `.mp4` inside BMP images, and later extracts the hidden data
successfully with the original file restored byte for byte. The project
uses bitwise operations, file handling, structures, and command-line
arguments to perform encoding and decoding efficiently.

The main objective of this project is to demonstrate secure data hiding
inside digital images while preserving the original appearance of the
image.

## Applications

- Secure Communication
- Digital Watermarking
- Data Protection
- Hidden Information Transfer
- Copyright Protection

## Features

- Encode secret files into BMP images
- Decode hidden files from stego images
- Uses the Least Significant Bit (LSB) technique
- Supports multiple secret file formats (`.txt`, `.jpg`, `.png`, `.pdf`, `.mp3`, `.mp4`)
- Automatic recovery of the secret file's extension on decode
- Preserves original image quality — no visible change to the cover image
- Capacity checking before encoding, to prevent silent data corruption
- Magic string verification required to decode
- Command-line based execution

## Project Structure

```
.
├── main.c
├── encode.c
├── encode.h
├── decode.c
├── decode.h
├── types.h
└── README.md
```

## Working Principle

### Encoding Process

1. Read the source BMP image
2. Read the secret file
3. Convert the secret file's data into binary bits
4. Store the secret bits inside the image's least significant bits
5. Generate the output stego image

### Decoding Process

1. Read the stego image
2. Extract the hidden bits from the image bytes
3. Reconstruct the secret data
4. Restore the original secret file, with its extension

## LSB Technique

The Least Significant Bit (LSB) technique stores secret data inside the
least significant bit of each byte in the image. Since only the last bit
of a byte is modified, the change to the pixel value is at most 1 (out of
255) — imperceptible to the human eye.

```
Original Byte : 1 0 1 1 0 1 1 0
Secret bit     : 1
Modified Byte  : 1 0 1 1 0 1 1 1
```

Only the last bit changes.

## Why BMP

BMP is an uncompressed image format, so pixel data is stored exactly as
written with no re-encoding on save. Compressed formats such as JPEG apply
lossy transformations that alter pixel values during compression, which
would destroy LSB-encoded data. This is why the cover image must be BMP;
the secret file has no such restriction.

**Capacity requirement:** the cover image must be at least 8 times larger
than the secret file, since each bit of the secret file requires one full
byte of the cover image to encode.

## Compilation

```bash
gcc -o steg main.c encode.c decode.c
```

## Usage

### Encoding

```bash
./steg -e <source.bmp> <secret_file> [output.bmp]
```

| Parameter | Description |
|---|---|
| `-e` | Encode operation |
| `source.bmp` | Cover image to encode data into |
| `secret_file` | File to hide (see supported types above) |
| `output.bmp` | Optional. Output filename, defaults to `stego.bmp` |

You will be prompted to enter a magic string at runtime. This string is
required again at decode time to authorize extraction.

### Decoding

```bash
./steg -d <stego.bmp> [output_filename]
```

| Parameter | Description |
|---|---|
| `-d` | Decode operation |
| `stego.bmp` | Encoded image to extract data from |
| `output_filename` | Optional. Output filename (extension restored automatically), defaults to `output` |

You will be prompted to re-enter the magic string used during encoding.

### Example

```bash
$ ./steg -e cover.bmp secret_message.png encoded.bmp
Enter the Magic String : my_secret_key
Data is Encoded Successfuly..!

$ ./steg -d encoded.bmp recovered
Enter Magic String to Varify : my_secret_key
Data is Decoded Successfuly..!
```

`recovered.png` is byte-for-byte identical to the original `secret_message.png`.

## Conclusion

This project demonstrates image steganography using the Least Significant
Bit (LSB) technique in C. The application hides and retrieves secret files
inside BMP images without noticeably affecting image quality. Building it
involved working with bitwise operations, file handling, structures, and
BMP image processing to implement a secure data-hiding technique from
first principles.

## Author

Narasimha S
