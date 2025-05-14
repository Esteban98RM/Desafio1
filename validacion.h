#ifndef VALIDACION_H
#define VALIDACION_H

#include "operaciones.h"

bool ValidarSumaMascara(unsigned char* imgTransformada, unsigned char* mask,
                        unsigned int* datosMascara, int semilla,
                        int anchoIMG, int altoIMG, int mask_ancho, int mask_alto);
bool validarXOR(unsigned char* actualIMG, unsigned char* IM, unsigned char* mask,
                unsigned int* datosMascara, int semilla,
                int anchoIMG, int altoIMG, int mask_ancho, int mask_alto);
bool validarRotarIzquierda(unsigned char* actualIMG, unsigned char* mask,
                           unsigned int* datosMascara, int semilla,
                           int anchoIMG, int altoIMG, int mask_ancho, int mask_alto, int bits);
bool validarRotarDerecha(unsigned char* actualIMG, unsigned char* mask,
                         unsigned int* datosMascara, int semilla,
                         int anchoIMG, int altoIMG, int mask_ancho, int mask_alto, int bits);

#endif // VALIDACION_H
