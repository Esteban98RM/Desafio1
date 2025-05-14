# TEMPLATE = app
# CONFIG += console c++17
# CONFIG -= app_bundle
# CONFIG -= qt

# SOURCES += \
#         main.cpp


QT += core gui
CONFIG += console c++17
SOURCES += main.cpp \
    operaciones.cpp \
    procesamiento.cpp \
    validacion.cpp

HEADERS += \
    operaciones.h \
    procesamiento.h \
    validacion.h
