#-------------------------------------------------
#
# Project created by QtCreator 2018-12-22T07:26:58
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

#QMAKE_CFLAGS += -std=c++11

TARGET = srh36ControlTest
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    srh_control_struc.h

FORMS    += mainwindow.ui

INCLUDEPATH += /usr/include/CCfits \
                /usr/include/cfitsio

LIBS += -lCCfits \
        -lcfitsio


