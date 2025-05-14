#include "validacion.h"
#include <iostream>

using namespace std;

/**
 * @brief Valida si una imagen transformada es el resultado de aplicar una suma modular con una máscara.
 *
 * Compara byte a byte la suma entre la imagen y la máscara contra los datos esperados en `datosMascara`.
 *
 * @param imgTransformada Imagen ya transformada (arreglo de bytes RGB).
 * @param mask Máscara aplicada (arreglo de bytes RGB).
 * @param datosMascara Valores esperados del resultado de la suma (enmascaramiento).
 * @param semilla Posición inicial donde comienza la aplicación de la máscara.
 * @param anchoIMG Ancho de la imagen.
 * @param altoIMG Alto de la imagen.
 * @param mask_ancho Ancho de la máscara.
 * @param mask_alto Alto de la máscara.
 * @return true Si todos los valores coinciden.
 * @return false Si hay alguna discrepancia.
 */

bool ValidarSumaMascara(unsigned char* imgTransformada, unsigned char* mask, unsigned int* datosMascara, int semilla, int anchoIMG, int altoIMG, int mask_ancho, int mask_alto) {
    if (!imgTransformada || !mask || !datosMascara) return false;

    int maskSize = mask_ancho * mask_alto * 3;
    int totalPixels = anchoIMG * altoIMG * 3;
    int pos = semilla;

    cout << "Validando suma mascara..." << endl;
    cout << "Posicion inicial: " << pos << endl;
    cout << "Dimension de la mascara: " << maskSize << endl;

    for (int k = 0; k < maskSize && pos + k < totalPixels; k++) {
        unsigned int suma = imgTransformada[pos + k] + mask[k];

        if (suma != datosMascara[k]) {
            cout << "Error en posicion " << k << ": esperado " << datosMascara[k]
                 << ", obtenido " << suma << endl;
            return false;
        }
    }

    cout << "Validacion exitosa!" << endl;
    return true;
}

/**
 * @brief Valida si la imagen actual se obtuvo mediante una operación XOR con una imagen intermedia (IM) seguida de suma de máscara.
 *
 * Primero aplica XOR entre `actualIMG` e `IM`, luego valida si el resultado coincide con los datos en `datosMascara` usando la máscara `mask`.
 *
 * @param actualIMG Imagen actual (transformada).
 * @param IM Imagen intermedia (antes del XOR).
 * @param mask Máscara usada para la validación.
 * @param datosMascara Datos de enmascaramiento esperados.
 * @param semilla Desplazamiento inicial en la imagen.
 * @param anchoIMG Ancho de la imagen.
 * @param altoIMG Alto de la imagen.
 * @param mask_ancho Ancho de la máscara.
 * @param mask_alto Alto de la máscara.
 * @return true Si la operación es válida.
 * @return false Si falla la validación.
 */

bool validarXOR(unsigned char* actualIMG, unsigned char* IM, unsigned char* mask, unsigned int* datosMascara, int semilla, int anchoIMG, int altoIMG, int mask_ancho, int mask_alto) {

    unsigned char* xorResult = DoXOR(actualIMG, IM, anchoIMG, altoIMG);
    bool esValido = ValidarSumaMascara(xorResult, mask, datosMascara, semilla, anchoIMG, altoIMG, mask_ancho, mask_alto);
    delete[] xorResult;
    return esValido;
}

/**
 * @brief Valida si una imagen fue generada aplicando rotación a la izquierda seguida de una suma con máscara.
 *
 * @param actualIMG Imagen actual.
 * @param mask Máscara usada para la suma.
 * @param datosMascara Datos esperados de la suma.
 * @param semilla Desplazamiento de inicio en la imagen.
 * @param anchoIMG Ancho de la imagen.
 * @param altoIMG Alto de la imagen.
 * @param mask_ancho Ancho de la máscara.
 * @param mask_alto Alto de la máscara.
 * @param bits Número de bits de rotación a la izquierda.
 * @return true Si la imagen es válida.
 * @return false En caso contrario.
 */

bool validarRotarIzquierda(unsigned char* actualIMG, unsigned char* mask, unsigned int* datosMascara, int semilla, int anchoIMG, int altoIMG, int mask_ancho, int mask_alto, int bits) {

    int totalPixeles = anchoIMG * altoIMG;
    unsigned char* rotado = RotarIzquierda(actualIMG, totalPixeles, bits);
    bool esValido = ValidarSumaMascara(rotado, mask, datosMascara, semilla, anchoIMG, altoIMG, mask_ancho, mask_alto);
    delete[] rotado;
    return esValido;
}

/**
 * @brief Valida si una imagen fue generada aplicando rotación a la derecha seguida de una suma con máscara.
 *
 * @param actualIMG Imagen actual.
 * @param mask Máscara usada para la suma.
 * @param datosMascara Datos esperados de la suma.
 * @param semilla Desplazamiento de inicio en la imagen.
 * @param anchoIMG Ancho de la imagen.
 * @param altoIMG Alto de la imagen.
 * @param mask_ancho Ancho de la máscara.
 * @param mask_alto Alto de la máscara.
 * @param bits Número de bits de rotación a la derecha.
 * @return true Si la imagen es válida.
 * @return false En caso contrario.
 */

bool validarRotarDerecha(unsigned char* actualIMG, unsigned char* mask, unsigned int* datosMascara, int semilla, int anchoIMG, int altoIMG, int mask_ancho, int mask_alto, int bits) {

    int totalPixeles = anchoIMG * altoIMG;
    unsigned char* rotado = RotarDerecha(actualIMG, totalPixeles, bits);
    bool esValido = ValidarSumaMascara(rotado, mask, datosMascara, semilla, anchoIMG, altoIMG, mask_ancho, mask_alto);
    delete[] rotado;
    return esValido;
}
