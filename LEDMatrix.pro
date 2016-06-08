#-------------------------------------------------
#
# Project created by QtCreator 2015-12-03T02:01:15
#
#-------------------------------------------------

QT       += core gui network

CONFIG   += c++11

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LEDMatrix
TEMPLATE = app


SOURCES += main.cpp\
        ledmatrix.cpp \
    qledmatrix.cpp \
    ledserver.cpp

HEADERS  += ledmatrix.h \
    qledmatrix.h \
    ledserver.h

FORMS    += ledmatrix.ui
