#-------------------------------------------------
#
# Project created by QtCreator 2018-09-16T19:42:35
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport

TARGET = AkiraMin20
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    qcustomplot.cpp

CONFIG += c++11

HEADERS  += mainwindow.h \
    qcustomplot.h

FORMS    += mainwindow.ui
