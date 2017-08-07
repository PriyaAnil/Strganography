#ifndef DECODE_H
#define DECODE_H

#include "types.h" // Contains user defined types

/* Structure to store information required for
 * encoding secret file to source Image
 * Info about output and intermediate data is
 * also stored
 */

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4

typedef struct _DecodeInfo
{
    /* Secret File Info */
    char *secret_fname;
    FILE *fptr_secret;
    char extn_secret_file[MAX_FILE_SUFFIX];
	uchar secret_extn_size;
    char secret_data[MAX_SECRET_BUF_SIZE];
    long size_secret_file;

    /* Stego Image Info */
    char *stego_image_fname;
    FILE *fptr_stego_image;

	/* Password Info */
	char password[10];
	uchar password_size;


} DecodeInfo;


/* Decoding function prototype */

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo, char *argv[], int argc);

/* Get File pointers for i/p and o/p files */
Status open_files_decode(DecodeInfo *decInfo);

/* Decode Magic String */
Status decode_magic_string(DecodeInfo *decInfo);

/* Decode  password  size */
Status decode_password_size(DecodeInfo *decInfo);

/* Decode password */
Status decode_password(DecodeInfo *decInfo);

/* Decode secret file extenstion */
Status decode_secret_file_extn(char *secret_fname, DecodeInfo *decInfo);

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo);

/* Decode secret file data*/
//Status decode_secret_file_data(DecodeInfo *decInfo);

/* Decode function, which does the real decoding */
Status decode_image_to_data(FILE *fptr_secret, FILE *fptr_stego_image, DecodeInfo *decInfo);

/* Decode a byte into LSB of image data array */
Status decode_lsb_tobyte(char *data, char *image_buffer);

#endif
