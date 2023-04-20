# S-DES_Image_Encryption
A project that uses S-DES encryption on a bitmap while preserving header information

# Bitmaps
For best results, a 24-bit bitmap should be used. This can be easily acquired by saving a file as a bitmap through Microsoft Paint
The 54 byte header is preserved, keeping image dimensions and linking intact


# Compilation 
For Encryption:
>G++ -o encrypt.exe encryption.cpp

>./encrypt.exe dog.bmp encryptedDog.bmp 3 4 

>Explorer.exe encryptedDog.bmp

For Decryption:

>G++ -o decrypt.exe decryption.cpp

>./decrypt.exe encryptedDog.bmp decryptedDog.bmp 3 4

>Explorer.exe decryptedDog.bmp

The program input arguments may be increased to take another input for the key, however the key is kept the same for testing purposes.
