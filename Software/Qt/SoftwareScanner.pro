include(qextserialport-1.2rc/src/qextserialport.pri)
#include(..\qextserialport-1.2rc\src\qextserialport.pri)

QT += core
QT += widgets

CONFIG -= app_bundle
#CONFIG += console
#CONFIG += static

TEMPLATE = app

SOURCES += mainwindow.cpp \
    main.cpp

HEADERS += mainwindow.h
