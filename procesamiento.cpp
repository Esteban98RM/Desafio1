#include "procesamiento.h"
#include <QImage>
#include <fstream>
#include <iostream>

#include "validacion.h"

using namespace std;

unsigned char* loadPixels(const QString& input, int& width, int& height) {
    QImage imagen(input);
    if (imagen.isNull()) {
        std::cout << "Error: No se pudo cargar la imagen." << std::endl;
        return nullptr;
    }

    imagen = imagen.convertToFormat(QImage::Format_RGB888);
    width = imagen.width();
    height = imagen.height();
    int dataSize = width * height * 3;

    unsigned char* pixelData = new unsigned char[dataSize];
    for (int y = 0; y < height; ++y) {
        memcpy(pixelData + y * width * 3, imagen.scanLine(y), width * 3);
    }

    return pixelData;
}

bool exportImage(unsigned char* pixelData, int width, int height, const QString& archivoSalida) {
    QImage outputImage(width, height, QImage::Format_RGB888);

    for (int y = 0; y < height; ++y) {
        memcpy(outputImage.scanLine(y), pixelData + y * width * 3, width * 3);
    }

    if (!outputImage.save(archivoSalida, "BMP")) {
        std::cout << "Error: No se pudo guardar la imagen BMP." << std::endl;
        return false;
    }
    return true;
}

unsigned int* loadSeedMasking(const char* nombreArchivo, int& seed, int& n_pixels) {
    std::ifstream archivo(nombreArchivo);
    if (!archivo.is_open()) {
        std::cout << "No se pudo abrir el archivo." << std::endl;
        return nullptr;
    }

    archivo >> seed;
    n_pixels = 0;
    int r, g, b;

    // Primera pasada para contar los píxeles
    while (archivo >> r >> g >> b) {
        n_pixels++;
    }

    archivo.close();
    archivo.open(nombreArchivo);

    if (!archivo.is_open()) {
        std::cout << "Error al reabrir el archivo." << std::endl;
        return nullptr;
    }

    unsigned int* RGB = new unsigned int[n_pixels * 3];
    archivo >> seed;

    for (int i = 0; i < n_pixels * 3; i += 3) {
        archivo >> r >> g >> b;
        RGB[i] = r;
        RGB[i + 1] = g;
        RGB[i + 2] = b;
    }

    archivo.close();
    std::cout << "Semilla: " << seed << std::endl;
    std::cout << "Cantidad de pixeles leidos: " << n_pixels << std::endl;

    return RGB;
}

void printOperationDescription(int operationCode) {
    switch (operationCode / 10) {
    case 0:
        cout << "XOR con I_M";
        break;
    case 2:
        cout << "Rotacion derecha (" << (operationCode%10) << " bits)";
        break;
    case 3:
        cout << "Rotacion izquierda (" << (operationCode%10) << " bits)";
        break;
    default:
        cout << "Operacion desconocida (Codigo: " << operationCode << ")";
    }
}

bool crearCopiaValidada(const QString& rutaBase) {
    // 1. Cargar imagen original I_O.bmp
    int width, height;
    QString originalPath = rutaBase + "I_O.bmp";
    unsigned char* IO = loadPixels(originalPath, width, height);

    if (!IO) {
        cerr << "Error: No se pudo cargar I_O.bmp" << endl;
        return false;
    }

    // 2. Cargar máscara M0.txt
    QString maskPath = rutaBase + "M0.txt";
    int seed, numPixels;
    unsigned int* maskData = loadSeedMasking(maskPath.toStdString().c_str(), seed, numPixels);

    if (!maskData) {
        cerr << "Error: No se pudo cargar M0.txt" << endl;
        delete[] IO;
        return false;
    }

    // 3. Cargar imagen de máscara M.bmp
    int mask_width, mask_height;
    QString maskImgPath = rutaBase + "M.bmp";
    unsigned char* M = loadPixels(maskImgPath, mask_width, mask_height);

    if (!M) {
        cerr << "Error: No se pudo cargar M.bmp" << endl;
        delete[] IO;
        delete[] maskData;
        return false;
    }

    // 4. Validar la suma de la máscara
    if (!ValidarSumaMascara(IO, M, maskData, seed, width, height, mask_width, mask_height)) {
        cerr << "Error: Validación de máscara fallida" << endl;
        delete[] IO;
        delete[] maskData;
        delete[] M;
        return false;
    }

    // 5. Crear copia validada
    QString copyPath = rutaBase + "I_OReconstruida.bmp";
    if (!exportImage(IO, width, height, copyPath)) {
        cerr << "Error: No se pudo guardar la copia" << endl;
        delete[] IO;
        delete[] maskData;
        delete[] M;
        return false;
    }

    //cout << "Copia validada creada exitosamente: " << copyPath.toStdString() << endl;

    // 6. Liberar memoria
    delete[] IO;
    delete[] maskData;
    delete[] M;

    return true;
}
