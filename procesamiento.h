#ifndef PROCESAMIENTO_H
#define PROCESAMIENTO_H

#include <QString>

unsigned char* loadPixels(const QString& input, int& width, int& height);
bool exportImage(unsigned char* pixelData, int width, int height, const QString& archivoSalida);
unsigned int* loadSeedMasking(const char* nombreArchivo, int& seed, int& n_pixels);
void printOperationDescription(int operationCode);
bool crearCopiaValidada(const QString& rutaBase);

#endif // PROCESAMIENTO_H
