#-------------------------------------------------
#
# Project created by QtCreator 2015-02-02T15:48:03
#
#-------------------------------------------------

QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = QYoutubeDownloader
TEMPLATE = app

linux {
    DESTDIR = $$_PRO_FILE_PWD_/build
    OBJECTS_DIR = $$_PRO_FILE_PWD_/build/.tmp
    MOC_DIR = $$_PRO_FILE_PWD_/build/.tmp
    RCC_DIR = $$_PRO_FILE_PWD_/build/.rcc
    UI_DIR = $$_PRO_FILE_PWD_/build/.ui
}

win32|win64 {
    DESTDIR = $$_PRO_FILE_PWD_\build
    DLLDESTDIR = $$_PRO_FILE_PWD_\build
    OBJECTS_DIR = $$_PRO_FILE_PWD_\build\tmp
    MOC_DIR = $$_PRO_FILE_PWD_\build\tmp
    RCC_DIR = $$_PRO_FILE_PWD_\build\rcc
    UI_DIR = $$_PRO_FILE_PWD_\build\ui
}

CONFIG += c++11

RESOURCES += style.qrc

SOURCES += main.cpp\
        mainwindow.cpp \
    dialognewdownload.cpp \
    settingswindow.cpp \
    queueitem.cpp \
    settings.cpp \
    sharedmemory.cpp \
    aboutwindow.cpp \
    osd.cpp

HEADERS  += mainwindow.h \
    dialognewdownload.h \
    settingswindow.h \
    queueitem.h \
    settings.h \
    sharedmemory.h \
    aboutwindow.h \
    osd.h

FORMS    += mainwindow.ui \
    dialognewdownload.ui \
    settingswindow.ui \
    aboutwindow.ui \
    osd.ui
