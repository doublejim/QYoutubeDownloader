#-------------------------------------------------
#
# Project created by QtCreator 2015-02-02T15:48:03
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = test
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    dialog1.cpp \
    ipc_client_server.cpp

HEADERS  += mainwindow.h \
    dialog1.h \
    ipc_client_server.h

FORMS    += mainwindow.ui \
    dialog1.ui
