#include "operaciones.h"
#include <iostream>
#include <cstring>

/**
 * @brief Aplica una operación XOR pixel a pixel entre dos imágenes.
 *
 * @param img1 Puntero a la primera imagen (arreglo de bytes RGB).
 * @param img2 Puntero a la segunda imagen (mismo tamaño que img1).
 * @param width Ancho de la imagen.
 * @param height Alto de la imagen.
 * @return unsigned char* Imagen resultante tras aplicar XOR. El puntero debe liberarse con `delete[]`.
 */

unsigned char* DoXOR(unsigned char* img1, unsigned char* img2, int width, int height) {
    if (!img1 || !img2) return nullptr;

    int totalPixels = width * height * 3;
    unsigned char* result = new unsigned char[totalPixels];
    if (!result) return nullptr;

    for (int i = 0; i < totalPixels; i++) {
        result[i] = img1[i] ^ img2[i];
    }
    return result;
}

/**
 * @brief Rota cada byte de una imagen hacia la derecha (bitwise) una cantidad de bits.
 *
 * @param img Imagen de entrada (arreglo de bytes RGB).
 * @param num_pixels Número total de píxeles de la imagen.
 * @param n Número de bits a rotar hacia la derecha.
 * @return unsigned char* Imagen resultante tras rotación. El puntero debe liberarse con `delete[]`.
 */

unsigned char* RotarDerecha(unsigned char* img, int num_pixels, int n) {
    unsigned char* result = new unsigned char[num_pixels * 3];
    for (int i = 0; i < num_pixels * 3; i++) {
        result[i] = (img[i] >> n) | (img[i] << (8 - n));
    }
    return result;
}

/**
 * @brief Rota cada byte de una imagen hacia la izquierda (bitwise) una cantidad de bits.
 *
 * @param img Imagen de entrada (arreglo de bytes RGB).
 * @param num_pixels Número total de píxeles de la imagen.
 * @param n Número de bits a rotar hacia la izquierda.
 * @return unsigned char* Imagen resultante tras rotación. El puntero debe liberarse con `delete[]`.
 */

unsigned char* RotarIzquierda(unsigned char* img, int num_pixels, int n) {
    unsigned char* result = new unsigned char[num_pixels * 3];
    for (int i = 0; i < num_pixels * 3; i++) {
        result[i] = (img[i] << n) | (img[i] >> (8 - n));
    }
    return result;
}

/**
 * @brief Aplica una suma modular (mod 256) entre una imagen y una máscara, comenzando desde un desplazamiento.
 *
 * Esta función copia la imagen original y luego suma los valores de la máscara a los bytes correspondientes,
 * comenzando desde una posición de desplazamiento específica.
 *
 * @param img Imagen original (arreglo de bytes RGB).
 * @param mask Máscara a sumar (arreglo de bytes RGB).
 * @param width Ancho de la imagen original.
 * @param height Alto de la imagen original.
 * @param mask_width Ancho de la máscara.
 * @param mask_height Alto de la máscara.
 * @param offset Desplazamiento dentro de la imagen donde se empezará a sumar la máscara.
 * @return unsigned char* Imagen resultante. El puntero debe liberarse con `delete[]`.
 */

unsigned char* SumarMascara(unsigned char* img, unsigned char* mask, int width, int height, int mask_width, int mask_height, int offset) {
    int totalPixels = width * height * 3;
    unsigned char* result = new unsigned char[totalPixels];

    // Copiar la imagen original al resultado
    for (int i = 0; i < totalPixels; i++) {
        result[i] = img[i];
    }

    int maskSize = mask_width * mask_height * 3;
    int pos = offset;

    for (int k = 0; k < maskSize && pos + k < totalPixels; k++) {
        // Suma con módulo 256 (sin overflow)
        result[pos + k] = (img[pos + k] + mask[k]) % 256;
    }

    return result;
}
