#include <stdio.h>
#include <string.h>
#include "decode.h"
#include "types.h"
#include "common.h"

/* Read and validate Encode args from argv */
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{

	decInfo -> stego_image_fname = argv[2];
	printf("%s\n", decInfo->stego_image_fname);
	decInfo -> secret_fname = argv[3];
	printf("%s\n", decInfo->secret_fname);

	return e_success;
}

/* Get File pointers for i/p and o/p files */
Status open_files_decode(DecodeInfo *decInfo)
{

	// Stego Image file
	decInfo->fptr_stego_image = fopen(decInfo->stego_image_fname, "r");

	//Do Error handling
	if (decInfo->fptr_stego_image == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->stego_image_fname);
		return e_failure;
	}

	// No failure return e_success
	return e_success;
}

/* Decode a LSB into byte of image data array */
Status decode_lsb_tobyte(char *data, char *image_buffer)
{

	int index, bit;

    *data = *data & 0;

    for (index = 0; index < 7; index++)
    {
		// Get bit 
        bit = image_buffer[index] & 1;
 		
		// Decode data
        *data = *data | (bit << index);
    }
	
	// No failure return e_success
	return e_success;
}

/* Decode Magic String */
Status decode_magic_string(DecodeInfo *decInfo)
{
	int byte_read, index;
	char image_buffer[10], magic_string[5];

	// Mover file pointer indicator by 54 bytes
	fseek(decInfo -> fptr_stego_image, 54L, SEEK_SET);

	for (index = 0; index < 2; index++)
	{

		// Read 8 bytes from stego image file
		byte_read = fread(image_buffer, 1, 8, decInfo -> fptr_stego_image);
		image_buffer[byte_read] = '\0';

		// Decode magic string
		decode_lsb_tobyte(&magic_string[index], image_buffer);
	}
	magic_string[index] ='\0';
	
	// Check for proper decoding
	if (strcmp(magic_string, MAGIC_STRING) == 0)
		return e_success;
	else
		return e_failure;
}

/* Decode  password  size */
Status decode_password_size(DecodeInfo *decInfo)
{
	int byte_read, index;
	char image_buffer[10];

	// Read 8 bytes from stego image file
	byte_read = fread(image_buffer, 1, 8, decInfo -> fptr_stego_image);
		image_buffer[byte_read] = '\0';

		// Decode password size
		decode_lsb_tobyte(&decInfo -> password_size, image_buffer);
		
		// No failure return e_success
		return e_success;
}

/* Decode  password */
Status decode_password(DecodeInfo *decInfo)
{
	
	int byte_read, index;
	char image_buffer[10], d_password[10];

	for (index = 0; index < decInfo -> password_size; index++)
	{

		// Read 8 bytes from stego image file
		byte_read = fread(image_buffer, 1, 8, decInfo -> fptr_stego_image);
		image_buffer[byte_read] = '\0';

		// Decode to password 
		decode_lsb_tobyte(&d_password[index], image_buffer);
	}
	d_password[index] = '\0';
	
	// Check for password matching
	if (strcmp(d_password, decInfo -> password) == 0)
		return e_success;
	else
	{
		printf("Invalid password\n");
		return e_failure;
	}
}
/* Decode secret file extenstion */
Status decode_secret_file_extn(char *secret_fname, DecodeInfo *decInfo)
{
	int byte_read, index = 0, j_index;
	char image_buffer[10], ch;

	// Read 8 bytes from stego image file 
	byte_read = fread(image_buffer, 1, 8, decInfo -> fptr_stego_image);
	image_buffer[byte_read] = '\0';

	// Decode secret file extention name size
	decode_lsb_tobyte(&decInfo -> secret_extn_size, image_buffer);

	while (decInfo -> secret_fname[index] != '\0')
	{
		index++;
	}

	// Copy '.' to secret file name
	decInfo -> secret_fname[index] = '.';
	index++;

	for (j_index = 0; j_index < decInfo -> secret_extn_size; j_index++)
	{

		// Read 8 bytes from stego image file
		byte_read = fread(image_buffer, 1, 8, decInfo -> fptr_stego_image);
		image_buffer[byte_read] = '\0';

		// Decode secret file extention name
		decode_lsb_tobyte(&ch, image_buffer);

		// Copy secret file extention name to secret file name
		decInfo -> secret_fname[index] = ch;
		index++;
	}
	
	decInfo -> secret_fname[index] = '\0';

	/* Open Secret file */
	// Secret file
	decInfo->fptr_secret = fopen(decInfo->secret_fname, "w");
	
	//Do Error handling
	if (decInfo->fptr_secret == NULL)
	{
		perror("fopen");
		fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->secret_fname);
		return e_failure;
	}
	// No failure return e_success
	return e_success;
}

/* Decode secret file size */
Status decode_secret_file_size(DecodeInfo *decInfo)
{
	int index;
	char image_buffer[10];

	decInfo -> size_secret_file = 0;

	unsigned char size = 0;

	for (index = 0; index < sizeof (long); index++)
	{

		// Read 8 bytes from output image file
		long byte_read = fread(image_buffer, 1, 8, decInfo -> fptr_stego_image);
		image_buffer[byte_read] = '\0';

		// Decode secret file size
		decode_lsb_tobyte(&size, image_buffer);
		
		// Get size byte by byte
		decInfo -> size_secret_file |= size;
		if (sizeof (long) - 1 != index)
			decInfo -> size_secret_file <<= (sizeof(char) * 8);
	}
	
	return e_success;
}

/* Decode function, which does the real decoding */
Status decode_image_to_data(FILE *fptr_secret, FILE *fptr_stego_image, DecodeInfo *decInfo)
{
	int index;
	char image_buffer[10], ch;

	for (index = 0; index < decInfo -> size_secret_file; index++)
	{
		// Read 8 bytes from output image file
		long byte_read = fread(image_buffer, 1, 8, decInfo -> fptr_stego_image);
		image_buffer[byte_read] = '\0';

		// Decode image to data 
		decode_lsb_tobyte(&ch, image_buffer);

		// Write to secret file
		fwrite(&ch, 1, 1, fptr_secret);
	}
	
	return e_success;
}

/* Perform the decoding */
Status do_decoding(DecodeInfo *decInfo, char *argv[], int argc)
{

	if (argc == 4)
	{		/* Read and validate Encode args from argv */
		if (read_and_validate_decode_args(argv, decInfo) != e_success)
		{
			printf("Error while validating\n");
			return e_failure;
		}
	}
	if (open_files_decode(decInfo) != e_success)
	{
		printf("Error while opening the files\n");
		return e_failure;
	}

	/* Decode Magic String */
	if (decode_magic_string(decInfo) != e_success)
	{
		printf("Error while decoding magic string\n");
		return e_failure;
	}
	
	/* Decode  password  size */
	if (decode_password_size(decInfo) != e_success)
	{
		printf("Error while decoding password size\n");
		return e_failure;
	}
	
	/* Decode password */
	if (decode_password(decInfo) != e_success)
	{
		printf("Error while decoding the password\n");
		return e_failure;
	}
	
	/* Decode secret file extention file name */
	if (decode_secret_file_extn(decInfo -> secret_fname, decInfo) != e_success)
	{
		printf("Error while decoding file extn\n");
		return e_failure;
	}
	
	/* Decode secret file size */
	if (decode_secret_file_size(decInfo) != e_success)
	{
		printf("Error while decoding secret file size\n");
		return e_failure;
	}

	/* Decode image to data */
 	if (decode_image_to_data(decInfo -> fptr_secret, decInfo -> fptr_stego_image, decInfo) != e_success)
	{
		printf("Error while decoding secret data\n");
		return e_failure;
	}

	return e_success;
}


