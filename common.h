#ifndef COMMON_H
#define COMMON_H

/* Magic string to identify whether stegged or not */
#define MAGIC_STRING "#*"

#endif
/* first we want to encode the data after gcc *.c ->./a.out -e beautiful.bmp secret.txt (in this file we can  store the secrect data)
this is minimum validation to encode the data to another BMP file(stego.bmp)(here we can give our own file name like bharath.bmp) or
else it will take automatically destination file(stego.bmp)we written code like that.

Decoding in stego.bmp the command-> ./a.out -d stego.bmp(destination file) we can give 
anyname.txt(here we can give our own file name like bharath.txt) or else automatically
take default name called final.txt(where secret data is decoded from stego.bmp)*/