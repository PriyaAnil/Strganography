#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "types.h"
#include "encode.h"
#include "decode.h"

/* Check operation type */
OperationType check_operation_type(char **argv)
{
	// For encoding
	if (strncmp(*argv, "-e", 2) == 0)
		return e_encode;

	// For decoding
	else if (strncmp(*argv, "-d", 2) == 0)
		return e_decode;

	else 
		return e_unsupported;
}
// Function for reading encode info
Status read_info_encode(EncodeInfo *encInfo)
{
	int size;
	char buff[50];

	// Read the source image file name
	printf("Enter the source image file name: ");
	scanf("%s", buff);
	size = strlen(buff);
	char *ptr = (char *)malloc(sizeof(char) * size);
	strcpy(ptr, buff);
	encInfo -> src_image_fname = ptr;

	// Reading the secret file name
	printf("Enter the secret file name: ");
	scanf("%s", buff);
	size = strlen(buff);
	ptr = malloc(sizeof(char) * size);
	strcpy(ptr, buff);
	encInfo -> secret_fname = ptr;

	// Reading the stego image file name
	printf("Enter the stego image file name: ");
	scanf("%s", buff);
	size = strlen(buff);
	ptr = malloc(sizeof(char) * size);
	strcpy(ptr, buff);
	encInfo -> stego_image_fname = ptr;

	return e_success;
}

// Function for reading decode info
Status read_info_decode(DecodeInfo *decInfo)
{
	int size;
	char buff[50];
	
	// Reading the stego image file name
	printf("Enter the stego image file name: ");
	scanf("%s", buff);
	size = strlen(buff);
	char *dptr = malloc(sizeof(char) * size);
	strcpy(dptr, buff);
	decInfo -> stego_image_fname = dptr;
	
	// Reading the secret file name
	printf("Enter the secret file name: ");
	scanf("%s", buff);
	size = strlen(buff);
	char *dptr1 = malloc(sizeof(char) * size);
	strcpy(dptr1, buff);
	decInfo -> secret_fname = dptr1;

	return e_success;
}

// Function for getting the password 
void get_password(EncodeInfo *encInfo, DecodeInfo *decInfo, int option)
{
	int size;
	char buffer[10];


	// For encoding
	if (option == 1)
	{
	
		// Reading password
		printf("Enter a password less than 8 characters: ");
		scanf("%s", buffer);
		size = strlen(buffer);

		if (size < 8)
		{
			strcpy(encInfo -> password, buffer);

			encInfo -> password_size = size;
		}
	}

	// For decoding
	else if (option == 2)
	{
		
		// Reading password
		printf("Enter the password: ");
		scanf("%s", buffer);
		size = strlen(buffer);
		
		strcpy(decInfo -> password, buffer);

		decInfo -> password_size = size;
	}

	else 
		printf("Check the password\n");
}

// Main program
int main(int argc, char *argv[])
{
	int option;
	EncodeInfo encInfo;
	DecodeInfo decInfo;
	
	// if argument count is 1
	if (argc == 1)
	{
		printf("Options:\n1. Encode\n2. Decode\nEnter the option: ");
		scanf("%d", &option);

		// For encoding
		if (option == 1)

			// Function call for reading encode info
			read_info_encode(&encInfo);

		// For decoding
		else if (option == 2)

			// Function call for reading decode info
			read_info_decode(&decInfo);

	}

	// For argument count more than 1
	else 
		option = check_operation_type(&argv[1]);
	

	switch (option)
	{
		case e_encode:

			// Function call for getting the password for encoding
			get_password(&encInfo, &decInfo, option);

			// Function call for encoding
			do_encoding(&encInfo, argv, argc);
			
			printf("Encoded Successfully!\n");
			
			break;

		case e_decode:

			// Function call for getting the password for decoding
			get_password(&encInfo, &decInfo, option);

			// Function call for decoding
			do_decoding(&decInfo, argv, argc);

			printf("Decoded Successfully!!\n");
			break;
	}
	return 0;
}

	
