#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */

/* Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */

/* Read and validate Encode args from argv */
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
		
	encInfo -> src_image_fname = argv[2];
	encInfo -> secret_fname = argv[3];
	encInfo -> stego_image_fname = argv[4];

	return e_success;
}

/* Open files */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    
	//Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);
    	return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    
	//Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);
    	return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    
	//Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
    	perror("fopen");
    	fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);
    	return e_failure;
    }

    // No failure return e_success
    return e_success;

}

/* Get image size for bmp */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    
	// Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
//    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
//    printf("height = %u\n", height);

	// Move file pointer indicator to initial position
    fseek(fptr_image, 0L, SEEK_SET);
    
	// Return image capacity
    return width * height * 3;
}

/* Get file size for secret file*/
uint get_file_size(FILE *fptr_src)
{
	long size_file;

	// Seek to EOF
	fseek(fptr_src, 0L, SEEK_END);
	
	// Get the size of the file
	size_file = ftell(fptr_src);

	// Move to initial position
	fseek(fptr_src, 0L, SEEK_SET);

	// Return file size
	return size_file;
}

/* Check capacity */
Status check_capacity(EncodeInfo *encInfo)
{

	// Get the file size of the secret file
	encInfo -> size_secret_file = get_file_size(encInfo -> fptr_secret);

	// Get the image capacity
	encInfo -> image_capacity = get_image_size_for_bmp(encInfo -> fptr_src_image); 

	// Check for size of image file and secret file
	if (encInfo -> image_capacity > encInfo -> size_secret_file)
    
		// No failure return e_success
		return e_success;
	else
	{
		printf("Error! Secret file size exceeds image file size\n");
		return e_failure; 
	}
}

/* Copy bmp image header */
Status copy_bmp_header(FILE * fptr_src_image, FILE *fptr_dest_image)
{
	long byte_read;
	char buffer[100];

	// Read 54 bytes from image file
	byte_read = fread(buffer, 1, 54, fptr_src_image);
	buffer[byte_read] = '\0';

	// Write 54 bytes to destination file
	long w_byte = fwrite(buffer, 1, byte_read, fptr_dest_image);
	
    // No failure return e_success
	return e_success;
}

/* Encode a byte into LSB of image data array */
Status encode_byte_tolsb(char data, char *image_buffer)
{
	int index, bit, data_bit;

	for (index = 0; index < 8; index++)
	{

		// Get bit from data
		bit = data & (1 << index);
		bit >>= index;
		data_bit = image_buffer[index] & 1;
		
		// If data_bit is 1
		if (data_bit == 1)

			// Flip the data_bit
			image_buffer[index] = image_buffer[index] & (~1);
		
		// Encode with image data	
		image_buffer[index] = image_buffer[index] | bit;
	}

    // No failure return e_success
	return e_success;
}

/* Store Magic String */
Status encode_magic_string(const char *magic_string, EncodeInfo *encInfo)
{
	int index;
	char image_buffer[10];

	for (index = 0; magic_string[index] != '\0'; index++)
	{
	
		// Read 8 bytes from image file
		long byte_read = fread(image_buffer, 1, 8, encInfo -> fptr_src_image);
		image_buffer[byte_read] = '\0';
		
		// Function call for encoding the magic string
		encode_byte_tolsb(magic_string[index], image_buffer);

		// Write to output image
		long w_byte = fwrite(image_buffer, 1, byte_read, encInfo -> fptr_stego_image);
	}
    
		// No failure return e_success
		return e_success;
}

/* Store password  size */
Status encode_password_size(EncodeInfo *encInfo)
{
	
	int index;
	char image_buffer[10];
		
	// Read 8 bytes from image file
	long byte_read = fread(image_buffer, 1, 8, encInfo -> fptr_src_image);
	image_buffer[byte_read] = '\0';

	// Function call for encoding the password size
	encode_byte_tolsb(encInfo -> password_size, image_buffer);
	
	// Write to output image
	fwrite(image_buffer, 1, byte_read, encInfo -> fptr_stego_image);
		
	// No failure return e_success
	return e_success;
}

/* Store password */
Status encode_password(EncodeInfo *encInfo)
{
	int index;
	char image_buffer[10];

	for (index = 0; encInfo -> password[index] != '\0'; index++)
	{
		
		// Read 8 bytes from image file
		long byte_read = fread(image_buffer, 1, 8, encInfo -> fptr_src_image);
		image_buffer[byte_read] = '\0';

		// Function call for encoding the password
		encode_byte_tolsb(encInfo -> password[index], image_buffer);

		// Write to output image
		long w_byte = fwrite(image_buffer, 1, byte_read, encInfo -> fptr_stego_image);
	}

	// No failure return e_success
	return e_success;
}

/* Encode secret file extenstion */
Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo)
{
	int index = 0;
	char image_buffer[10];

	// Getting the last occurance of the '.'
	char *cptr = strrchr(file_extn, '.');
	
	// Copying the file extention name 
	strcpy(encInfo -> extn_secret_file, ++cptr);
	
	// Calculating the size of the file extention name
	encInfo -> secret_extn_size = strlen(encInfo -> extn_secret_file);
	
	// Read 8 bytes from image file
	long byte_read = fread(image_buffer, 1, 8, encInfo -> fptr_src_image);
	image_buffer[byte_read] = '\0';
	
	// Function call to encode secret file extention size 
	encode_byte_tolsb(encInfo -> secret_extn_size, image_buffer);
	
	// Write to the output image file
	long w_byte = fwrite(image_buffer, 1, byte_read, encInfo -> fptr_stego_image);

	for (index = 0; encInfo->extn_secret_file[index] != '\0'; index++)
	{
		// Read 8 bytes from source image file
		byte_read = fread(image_buffer, 1, 8, encInfo -> fptr_src_image);
		image_buffer[byte_read] = '\0';
	
		// Function call to encode secret extention file name
		encode_byte_tolsb(encInfo -> extn_secret_file[index], image_buffer);
	
		// Write to output image file
		w_byte = fwrite(image_buffer, 1, byte_read, encInfo -> fptr_stego_image);
	}

	// No failure return e_success
	return e_success;
}

/* Encode secret file size */
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
	int size, bit, index, j_index;
	char ch, image_buffer[10];
	unsigned int num = 0;
	

	long total_size = encInfo -> size_secret_file;
	size = sizeof(long) * 8;

	// Get byte
	for (index = 0; index < sizeof(long); index++)
	{
		ch = '\0';
		num = 0;
		
		// Get bit
		for (j_index = 0; j_index < 8; j_index++)
		{
			size--;

			bit = total_size & (1 << size);

			num = num | bit;
		}
	
		num >>= size;
		ch |= num;
		
		// Read 8 bytes from source image file
		long byte_read = fread(image_buffer, 1, 8, encInfo -> fptr_src_image);
		image_buffer[byte_read] = '\0';

		// Function call for encoding secret file size
		encode_byte_tolsb(ch, image_buffer);

		// Write to output image file
		long w_byte = fwrite(image_buffer, 1, byte_read, encInfo -> fptr_stego_image);
	}

	// No failure return e_success
	return e_success;
}

/* Encode function, which does the real encoding */
Status encode_data_to_image(FILE *fptr_secret, FILE *fptr_src_image, FILE *fptr_stego_image, EncodeInfo *encInfo)
{
	int byte_read_sec, byte_read_img, index;
	char image_buffer[10];

	// Check until EOF
	while (feof(fptr_secret) == 0)
	{

		// Read 8 bytes from source image file
		byte_read_sec = fread(encInfo -> secret_data, 1, 8, fptr_secret);
		encInfo -> secret_data[byte_read_sec] = '\0';

		for (index = 0; encInfo -> secret_data[index] != '\0'; index++)
		{
			
			// Read 8 bytes from source image file
			byte_read_img = fread(image_buffer, 1, 8, fptr_src_image);
			image_buffer[byte_read_img] = '\0';

			// FUnction call for encoding the secret data
			encode_byte_tolsb(encInfo -> secret_data[index], image_buffer);

			// Write to the output image file
			fwrite(image_buffer, 1, byte_read_img, fptr_stego_image);
		}
	}

	// No failure return e_success
	return e_success;
}

/* Copy remaining image bytes from src to stego image after encoding */
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
	long byte_read;
	char image_buffer[1000001];

	// Check until EOF
	while (feof(fptr_src) == 0)
	{

		// Read 1000000 bytes from source image file
		byte_read = fread(image_buffer, 1, 1000000, fptr_src);
		image_buffer[byte_read] = '\0';

		// Write to output image file
		fwrite(image_buffer, 1, byte_read, fptr_dest);
	}

	// No failure return e_success
	return e_success;
}

/* Perform the encoding */
Status do_encoding(EncodeInfo *encInfo, char *argv[], int argc)
{
	if (argc == 5)
	{
		
		/* Read and validate Encode args from argv */
		if (read_and_validate_encode_args(argv, encInfo) != e_success)
			printf("Error while validating\n");
	}

	/* Open files */
	if (open_files(encInfo) != e_success)
		printf("Error while opening the files\n");
	
	/* Check capacity */
	if (check_capacity(encInfo) != e_success)
		printf("Error while checking capacity\n");

	/* Copy bmp image header */
	if (copy_bmp_header(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) != e_success)
		printf("Error while copying bmp header\n");

	/* Store Magic String */
	if (encode_magic_string(MAGIC_STRING, encInfo) != e_success)
		printf("Error while encoding magic string\n");

	/* Store password  size */
	if (encode_password_size(encInfo) != e_success)
		printf("Error while encoding password size\n");

	/* Store password */
	if (encode_password(encInfo) != e_success)
		printf("Error while encoding the password\n");

	/* Encode secret file extenstion */
	if (encode_secret_file_extn(encInfo -> secret_fname, encInfo) != e_success)
		printf("Error while encoding file extn\n");

	/* Encode secret file size */
	if (encode_secret_file_size(encInfo -> size_secret_file, encInfo) != e_success)
		printf("Error while encoding secret file size\n");

	/* Encode function, which does the real encoding */
 	if (encode_data_to_image(encInfo -> fptr_secret, encInfo ->fptr_src_image, encInfo -> fptr_stego_image, encInfo) != e_success)
		printf("Error while encoding secret data\n");

	/* Copy remaining image bytes from src to stego image after encoding */
	if (copy_remaining_img_data(encInfo -> fptr_src_image, encInfo -> fptr_stego_image) != e_success)
		printf("Error while copying remaining image data\n");

	// No failure return e_success
	return e_success;
}

