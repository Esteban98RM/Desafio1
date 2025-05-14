#ifndef OPERACIONES_H
#define OPERACIONES_H

const int MAX_BITS = 8;

unsigned char* DoXOR(unsigned char* img1, unsigned char* img2, int width, int height);
unsigned char* RotarDerecha(unsigned char* img, int num_pixels, int n);
unsigned char* RotarIzquierda(unsigned char* img, int num_pixels, int n);
unsigned char* SumarMascara(unsigned char* img, unsigned char* mask, int width, int height, int mask_width, int mask_height, int offset);

#endif // OPERACIONES_H
