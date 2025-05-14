#include <iostream>
#include <fstream>
#include <QCoreApplication>
#include <QImage>

#include "operaciones.h"
#include "validacion.h"
#include "procesamiento.h"

using namespace std;

// Prototipos de funciones
int DeterminarOperacionInversa(unsigned char* actualIMG, unsigned char* IM, unsigned char* M, unsigned int* datosMascara, int semilla, int anchoIMG, int altoIMG, int mask_ancho, int mask_alto);
bool cargarDatosBase(const QString& rutaBase, int& anchoIMG, int& altoIMG, int& mask_ancho, int& mask_alto, unsigned char*& ID, unsigned char*& IM, unsigned char*& IO, unsigned char*& M);
bool cargarDatosEnmascaramiento(const QString& rutaBase, int numEtapas, unsigned int**& datosMascara, int*& semilla, int*& numPixels);
unsigned char* aplicarOperacionInversa(unsigned char* actualIMG, unsigned char* IM, int operation, int anchoIMG, int altoIMG);
bool procesarEtapa(int etapa, int numEtapas, unsigned char*& currentImg,unsigned char* ID, unsigned char* IO, unsigned char* IM, unsigned char* M, unsigned int** maskingData, int* seeds, int width, int height, int mask_width, int mask_height, int* operations, const QString& rutaBase);
void reconstruirImagen(const QString& rutaBase, int numEtapas);

// Implementacion de funciones

/**
 * @brief Determina que operacion inversa fue aplicada a una imagen distorsionada.
 *
 * Esta funcion intenta deducir que tipo de operacion fue utilizada para distorsionar una imagen
 * comparando la imagen actual con varias versiones generadas a partir de la mascara.
 * Evalua primero XOR, luego rotaciones hacia la izquierda y derecha de 1 a MAX_BITS (8) bits.
 *
 * @param actualIMG Puntero a la imagen distorsionada actual.
 * @param IM Puntero a la imagen para aplicar XOR.
 * @param M Puntero a la imagen de mascara.
 * @param datosMascara Puntero a datos auxiliares de la mascara.
 * @param semilla Semilla utilizada para reproducir la operacion (por ejemplo, en PRNGs).
 * @param anchoIMG Ancho de la imagen en pixeles.
 * @param altoIMG Alto de la imagen en pixeles.
 * @param mask_ancho Ancho de la mascara en pixeles.
 * @param mask_alto Alto de la mascara en pixeles.
 *
 * @return int Codigo que representa la operacion inversa detectada:
 *         - 1  → Operacion XOR.
 *         - 2X → Rotacion a la izquierda de X bits.
 *         - 3X → Rotacion a la derecha de X bits.
 *         - -1 → No se pudo determinar la operacion.
 *
 * @see validarXOR, validarRotarIzquierda, validarRotarDerecha
 */

int DeterminarOperacionInversa(unsigned char* actualIMG, unsigned char* IM, unsigned char* M, unsigned int* datosMascara, int semilla, int anchoIMG, int altoIMG, int mask_ancho, int mask_alto) {

    // 1. Validar XOR primero (operacion mas comun)
    cout << "Validando operacion XOR..." << endl;
    if (validarXOR(actualIMG, IM, M, datosMascara, semilla, anchoIMG, altoIMG, mask_ancho, mask_alto)) {
        cout << "Operacion XOR validada correctamente" << endl;
        return 1; // Codigo para XOR
    }

    // 2. Validar rotaciones (de 1 a MAX_BITS bits)
    cout << "Validando operaciones de rotacion..." << endl;
    for (int bits = 1; bits <= MAX_BITS; bits++) {
        // Rotacion izquierda (inverso de rotacion derecha original)
        cout << "Validando rotacion izquierda de " << bits << " bits..." << endl;
        if (validarRotarIzquierda(actualIMG, M, datosMascara, semilla, anchoIMG, altoIMG, mask_ancho, mask_alto, bits)) {
            cout << "Rotacion izquierda de " << bits << " bits validada" << endl;
            return 20 + bits; // Codigo 2X para rotacion izquierda (X = bits)
        }

        // Rotacion derecha (inverso de rotacion izquierda original)
        cout << "Validando rotacion derecha de " << bits << " bits..." << endl;
        if (validarRotarDerecha(actualIMG, M, datosMascara, semilla, anchoIMG, altoIMG, mask_ancho, mask_alto, bits)) {
            cout << "Rotacion derecha de " << bits << " bits validada" << endl;
            return 30 + bits; // Codigo 3X para rotacion derecha (X = bits)
        }
    }

    // 3. Si no se valido ninguna operacion
    cerr << "Error: No se pudo determinar la operacion inversa" << endl;
    return -1;
}

/**
 * @brief Carga las imagenes necesarias desde disco para procesar la reconstruccion o analisis.
 *
 * Esta funcion carga las imagenes involucradas en la transformacion:
 * - M: Imagen de mascara.
 * - I_M: Imagen para aplicar XOR.
 * - I_D: Imagen distorsionada.
 * - I_O: Imagen original sin modificar.
 *
 * Verifica que cada imagen se haya cargado correctamente y que las dimensiones sean validas.
 *
 * @param rutaBase Ruta base donde se encuentran las imagenes.
 * @param anchoIMG Referencia al ancho de las imagenes base.
 * @param altoIMG Referencia al alto de las imagenes base.
 * @param mask_ancho Referencia al ancho de la imagen de mascara.
 * @param mask_alto Referencia al alto de la imagen de mascara.
 * @param ID Referencia al puntero que contendra la imagen distorsionada.
 * @param IM Referencia al puntero que contendra la imagen para aplicar operacion XOR.
 * @param IO Referencia al puntero que contendra la imagen original.
 * @param M Referencia al puntero que contendra la mascara.
 *
 * @return true Si todas las imagenes fueron cargadas exitosamente y las dimensiones son validas.
 * @return false Si ocurrio algun error al cargar una o mas imagenes.
 *
 * @see loadPixels
 */

bool cargarDatosBase(const QString& rutaBase, int& anchoIMG, int& altoIMG, int& mask_ancho, int& mask_alto, unsigned char*& ID, unsigned char*& IM, unsigned char*& IO, unsigned char*& M) {
    // Cargar imagen mascara M
    QString mascaraPath = rutaBase + "M.bmp";
    cout << "Cargar mascara M : M.bmp" << endl;
    M = loadPixels(mascaraPath, mask_ancho, mask_alto);

    if (!M || mask_ancho == 0 || mask_alto == 0) {
        cerr << "Error: No se pudo cargar la mascara M o dimensiones invalidas" << endl;
        return false;
    }
    cout << "Mascara M cargada correctamente. Dimensiones: "
         << mask_ancho << "x" << mask_alto << endl;

    // Cargar imagenes base
    QString imPath = rutaBase + "I_M.bmp";
    QString idPath = rutaBase + "I_D.bmp";
    QString ioPath = rutaBase + "I_O.bmp";

    cout << "Imagen para XOR IM : I_M.bmp" << endl;
    IM = loadPixels(imPath, anchoIMG, altoIMG);

    cout << "Imagen original distorcionada ID : I_D.bmp" << endl;
    ID = loadPixels(idPath, anchoIMG, altoIMG);

    cout << "Imagen original IO : I_O.bmp" << endl;
    IO = loadPixels(ioPath, anchoIMG, altoIMG);


    if (!IM || !ID || !IO) {
        cerr << "Error al cargar imagenes base" << endl;
        if (IM) delete[] IM;
        if (ID) delete[] ID;
        if (IO) delete[] IO;
        if (M) delete[] M;
        return false;
    }

    cout << "Imagenes base cargadas correctamente. Dimensiones: "
         << anchoIMG << "x" << altoIMG << endl;

    return true;
}

/**
 * @brief Carga los archivos de datos de enmascaramiento para multiples etapas.
 *
 * Esta funcion lee archivos de texto con extension `.txt` que contienen los datos
 * de enmascaramiento necesarios para aplicar o revertir operaciones sobre imagenes.
 * Asigna dinamicamente memoria para almacenar:
 * - Los datos de la mascara (`datosMascara`)
 * - Las semillas asociadas a cada etapa (`semilla`)
 * - El numero de pixeles utilizados en cada etapa (`numPixels`)
 *
 * @param rutaBase Ruta base donde se encuentran los archivos.
 * @param numEtapas Numero total de etapas o archivos a cargar (ej. M1.txt, M2.txt, ...).
 * @param datosMascara Referencia a un puntero doble que almacenara los datos por etapa.
 * @param semilla Referencia a un arreglo que contendra las semillas asociadas a cada etapa.
 * @param numPixels Referencia a un arreglo que contendra el numero de pixeles por etapa.
 *
 * @return true Si todos los archivos fueron cargados correctamente.
 * @return false Si alguno de los archivos no pudo ser cargado (libera memoria asignada).
 *
 * @see loadSeedMasking
 */

bool cargarDatosEnmascaramiento(const QString& rutaBase, int numEtapas, unsigned int**& datosMascara, int*& semilla, int*& numPixels) {

    datosMascara = new unsigned int*[numEtapas];
    semilla = new int[numEtapas];
    numPixels = new int[numEtapas];

    for (int i = 0; i < numEtapas; i++) {
        QString filename = rutaBase + "M" + QString::number(i+1) + ".txt";
        datosMascara[i] = loadSeedMasking(filename.toStdString().c_str(), semilla[i], numPixels[i]);

        if (!datosMascara[i]) {
            cerr << "Error al cargar archivo de enmascaramiento " << i+1 << endl;
            // Limpiar memoria ya asignada
            for (int j = 0; j < i; j++) delete[] datosMascara[j];
            delete[] datosMascara;
            delete[] semilla;
            delete[] numPixels;
            return false;
        }
    }
    return true;
}

/**
 * @brief Aplica la operacion inversa a una imagen distorsionada, dependiendo del tipo de transformacion detectada.
 *
 * Esta funcion realiza la operacion inversa sobre una imagen `actualIMG`, utilizando
 * informacion de la imagen `IM` y el codigo de operacion proporcionado. Soporta:
 * - XOR inverso (usando la misma imagen XOR).
 * - Rotacion izquierda (para revertir rotaciones derechas).
 * - Rotacion derecha (para revertir rotaciones izquierdas).
 *
 * @param actualIMG Puntero a la imagen distorsionada.
 * @param IM Puntero a la imagen auxiliar utilizada para revertir la operacion (solo para XOR).
 * @param operation Codigo de operacion inversa detectado (por ejemplo, 1 para XOR, 22 para rotacion izq. de 2 bits).
 * @param anchoIMG Ancho de la imagen en pixeles.
 * @param altoIMG Alto de la imagen en pixeles.
 *
 * @return unsigned char* Puntero a la imagen resultante tras aplicar la operacion inversa.
 *
 * @see DoXOR, RotarIzquierda, RotarDerecha
 */

unsigned char* aplicarOperacionInversa(unsigned char* actualIMG, unsigned char* IM, int operation, int anchoIMG, int altoIMG) {
    unsigned char* result = nullptr;

    switch (operation / 10) {
    case 0: // XOR
        cout << "Aplicando XOR inverso" << endl;
        result = DoXOR(actualIMG, IM, anchoIMG, altoIMG);
        break;

    case 2: // Rotacion derecha original → izquierda inversa
        cout << "Aplicando rotacion izquierda de " << operation%10 << " bits" << endl;
        result = RotarIzquierda(actualIMG, anchoIMG * altoIMG, operation%10);
        break;

    case 3: // Rotacion izquierda original → derecha inversa
        cout << "Aplicando rotacion derecha de " << operation%10 << " bits" << endl;
        result = RotarDerecha(actualIMG, anchoIMG * altoIMG, operation%10);
        break;

    default:
        cerr << "Operacion desconocida: " << operation << endl;
        break;
    }

    return result;
}

/**
 * @brief Procesa una etapa de reconstruccion aplicando la operacion inversa correspondiente.
 *
 * Esta funcion guarda la imagen actual, determina que operacion de distorsion fue aplicada en la etapa,
 * aplica la operacion inversa, actualiza la imagen y guarda una reconstruccion intermedia.
 *
 * @param etapa indice de la etapa actual (en orden inverso).
 * @param numEtapas Numero total de etapas de enmascaramiento.
 * @param currentImg Imagen actual que se esta reconstruyendo (actualizada por referencia).
 * @param ID Imagen distorsionada original.
 * @param IO Imagen original sin modificar.
 * @param IM Imagen utilizada para operaciones XOR.
 * @param M Imagen de mascara (BMP).
 * @param maskingData Arreglo doble con los datos de enmascaramiento por etapa.
 * @param seeds Arreglo de semillas utilizadas en cada etapa.
 * @param width Ancho de la imagen.
 * @param height Alto de la imagen.
 * @param mask_width Ancho de la mascara.
 * @param mask_height Alto de la mascara.
 * @param operations Arreglo para almacenar las operaciones detectadas.
 * @param rutaBase Ruta base donde se guardaran las imagenes intermedias.
 *
 * @return true Si la operacion inversa fue aplicada y la imagen reconstruida correctamente.
 * @return false Si ocurre un error durante el procesamiento o deteccion de la operacion.
 *
 * @see DeterminarOperacionInversa, aplicarOperacionInversa, exportImage
 */

bool procesarEtapa(int etapa, int numEtapas, unsigned char*& currentImg, unsigned char* ID, unsigned char* IO, unsigned char* IM, unsigned char* M, unsigned int** maskingData, int* seeds, int width, int height, int mask_width, int mask_height, int* operations, const QString& rutaBase) {

    // Mostrar informacion clara de la etapa actual
    cout << "\n=== PROCESANDO ETAPA " << (numEtapas - etapa) << "/" << numEtapas << " ===" << endl;
    cout << "Archivo de entrada: P" << (etapa+1) << ".bmp" << endl;

    // 1. Exportar la imagen actual como P(etapa+1).bmp
    QString nombreImagen = rutaBase + QString("P%1.bmp").arg(etapa+1);
    if (!exportImage(currentImg, width, height, nombreImagen)) {
        cerr << "Error al exportar imagen P" << etapa+1 << ".bmp" << endl;
        return false;
    }

    // 2. Determinar que operacion se aplico en esta etapa
    int operacion = DeterminarOperacionInversa(currentImg, IM, M, maskingData[etapa], seeds[etapa], width, height, mask_width, mask_height);
    if (operacion == -1) {
        cerr << "No se pudo determinar la operacion para la etapa " << etapa << endl;
        return false;
    }

    // Guardar la operacion detectada para mostrar al final
    operations[etapa] = operacion;

    // 3. Aplicar la operacion inversa para obtener la imagen anterior
    unsigned char* nuevaImagen = nullptr;

    if (operacion == 1) { // XOR
        nuevaImagen = aplicarOperacionInversa(currentImg, IM, operacion, width, height);
    } else { // Rotaciones
        nuevaImagen = aplicarOperacionInversa(currentImg, nullptr, operacion, width, height);
    }

    if (!nuevaImagen) {
        cerr << "Error al aplicar operacion inversa en etapa " << etapa << endl;
        return false;
    }

    // 4. Actualizar imagen y guardar reconstruccion
    if (currentImg != ID) {
        delete[] currentImg;
    }
    currentImg = nuevaImagen;

    QString nombreReconstruida = rutaBase + QString("P%1_reconstruida.bmp").arg(etapa);
    if (!exportImage(currentImg, width, height, nombreReconstruida)) {
        cerr << "ADVERTENCIA: No se pudo guardar reconstruccion intermedia" << endl;
    }

    cout << "=== ETAPA " << (numEtapas - etapa) << " COMPLETADA ===" << endl;
    cout << "==========================" << endl;

    return true;

}

/**
 * @brief Reconstruye la imagen original a partir de una version distorsionada mediante varias etapas de enmascaramiento.
 *
 * Esta funcion orquesta el proceso completo de reconstruccion:
 * - Carga las imagenes base y la mascara.
 * - Carga los datos de enmascaramiento para todas las etapas.
 * - Aplica operaciones inversas en orden inverso a la distorsion.
 * - Guarda las imagenes intermedias y la imagen final reconstruida.
 * - Muestra un resumen de operaciones aplicadas.
 *
 * @param rutaBase Ruta base donde se encuentran las imagenes y se guardaran los resultados.
 * @param numEtapas Numero de etapas de enmascaramiento aplicadas.
 *
 * @see cargarDatosBase, cargarDatosEnmascaramiento, procesarEtapa, exportImage, printOperationDescription
 */

void reconstruirImagen(const QString& rutaBase, int numEtapas) {
    // 1. Cargar datos base
    int width, height, mask_width, mask_height;
    unsigned char *IM, *ID, *IO, *M;

    if (!cargarDatosBase(rutaBase, width, height, mask_width, mask_height, ID, IM, IO, M)) {
        return;
    }

    // 2. Cargar datos de enmascaramiento
    unsigned int** maskingData;
    int *seeds, *numPixels;

    if (!cargarDatosEnmascaramiento(rutaBase, numEtapas, maskingData, seeds, numPixels)) {
        delete[] IM;
        delete[] ID;
        delete[] IO;
        delete[] M;
        return;
    }

    // 3. Preparar reconstruccion
    unsigned char* currentImg = ID;
    int* operations = new int[numEtapas];
    bool success = true;

    // 4. Procesar cada etapa en orden inverso con mejor feedback
    cout << "\nINICIANDO RECONSTRUCCION (" << numEtapas << " etapas)\n" << endl;

    for (int etapa = numEtapas-1; etapa >= 0; etapa--) {
        cout << ">> Procesando etapa " << (numEtapas - etapa)
        << " (archivo P" << (etapa+1) << ".bmp)" << endl;

        if (!procesarEtapa(etapa, numEtapas, currentImg, ID, IO, IM, M, maskingData, seeds, width, height, mask_width, mask_height, operations, rutaBase)) {
            success = false;
            cerr << "!! RECONSTRUCCION FALLIDA EN ETAPA " << (numEtapas - etapa) << endl;
            break;
        }
    }


    if (success) {
        if (!crearCopiaValidada(rutaBase)) {
            cerr << "Advertencia: No se pudo crear la copia validada" << endl;
        }
    }

    // 5. Guardar resultado final con verificacion

    if (success) {
        QString finalPath = rutaBase + "I_0Reconstruida.bmp";
        if (exportImage(currentImg, width, height, finalPath)) {
            cout << "\nRECONSTRUCCION EXITOSA!" << endl;

            // Mostrar resumen ordenado inversamente
            cout << "\nRESUMEN DE OPERACIONES:" << endl;
            cout << "Orden reconstruido (de ultima a primera aplicacion):" << endl;
            for (int i = numEtapas - 1; i >= 0; i--) {
                cout << "Etapa " << (numEtapas - i) << ": ";
                printOperationDescription(operations[i]);
                cout << endl;
            }

            // Añadir una etapa XOR como ultima
            cout << "Etapa " << (numEtapas + 1) << ": ";
            printOperationDescription(1);
            cout << endl;

        } else {
            cerr << "ERROR: No se pudo guardar la imagen final" << endl;
        }
    }

    // 6. Liberar memoria
    delete[] IM;
    delete[] ID;
    delete[] IO;
    delete[] M;
    for (int i = 0; i < numEtapas; i++) {
        delete[] maskingData[i];
    }
    delete[] maskingData;
    delete[] seeds;
    delete[] numPixels;
    delete[] operations;
}

int main() {

    // Configuracion de parametros
    QString rutaBase = "C:/Users/esteb/OneDrive/Escritorio/DES/codigo/Desafio1/Caso 2/";
    int numEtapas = 6; // Cambiar segun el numero de etapas que tenga tu caso

    // Mensaje inicial

    cout << "=============================================" << endl;
    cout << "  SISTEMA DE RECONSTRUCCION DE IMAGENES" << endl;
    cout << "=============================================" << endl;
    cout << "Ruta base: " << rutaBase.toStdString() << endl;
    cout << "Numero de etapas: " << numEtapas + 1 << endl;
    cout << "Iniciando proceso..." << endl;

    // Llamar a la funcion principal
    reconstruirImagen(rutaBase, numEtapas);

    cout << "Proceso completado" << endl;
    return 0;
}
