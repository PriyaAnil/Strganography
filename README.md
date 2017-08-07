# Strganography
The objective is to send a secret text file encoded inside an image of bmp file format. Encoded the length of the secret text and then encoded the data into the LSB of the image bytes. The decoding process involves decoding the length and then decoding the text bit by bit. The final output is the secret text after decoding.

ALGORITHM : Encoding

start
Encoding:
Read files : 
Text file
Source image file
Destination image file
Create password
Open all files
Check capacity of source file, Source file size is not enough for secret file then print error and Stop
Copy header file from source image to destination image
Encode magic string
Encode secret file password size and password
Encode secret file extension
Encode secret file size
Encode secret file data
Copy remaining bytes from source image file to destination image file
Stop

ALGORITHM : Decoding

Start
Decoding:
Read files : 
Text file
Encoded image file
Read the password
Open all files
Decode magic string
Decode secret file password size and password
Compare decoded password with user entered passsword , if they are not matching then print error and Stop 
Decode secret file extension
Decode secret file size
Decode secret file data
Print secret information to user or store the information to new text file
Stop

USAGE: ./a.out steg_main.c encode.c decode.c



