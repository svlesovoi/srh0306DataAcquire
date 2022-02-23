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
    qcustomplot.cpp \
    qsoldatfile.cpp \
    qg7m.cpp

HEADERS  += mainwindow.h \
    qcustomplot.h \
    correlator.h \
    qsoldatfile.h \
    visatype.h \
    visa.h \
    qg7m.h

FORMS    += mainwindow.ui

INCLUDEPATH += /usr/include/CCfits \
                /usr/include/cfitsio

LIBS += -lCCfits \
        -lcfitsio \
        -L /home/svlesovoi/workspace/srh36ControlTest/ -lMiVisa


