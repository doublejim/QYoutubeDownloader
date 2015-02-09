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
    ipc_client_server.cpp \
    dialognewdownload.cpp \
    settingswindow.cpp \
    queueitem.cpp

HEADERS  += mainwindow.h \
    ipc_client_server.h \
    dialognewdownload.h \
    settingswindow.h \
    queueitem.h

FORMS    += mainwindow.ui \
    dialognewdownload.ui \
    settingswindow.ui
