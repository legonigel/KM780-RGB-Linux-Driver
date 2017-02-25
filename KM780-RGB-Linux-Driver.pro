#-------------------------------------------------
#
# Project created by QtCreator 2016-07-31T01:40:26
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = KM780-RGB-Linux-Driver
TEMPLATE = app

CONFIG += c++11

OBJECTS_DIR = .obj

DESTDIR = bin
MOC_DIR = .moc
UI_DIR = .ui

SOURCES +=\
    src/keyboard.cpp \
    src/main.cpp \
    src/mainwindow.cpp

HEADERS  += src/keyboard.hpp \
    src/main.hpp \
    src/mainwindow.hpp

FORMS    += src/mainwindow.ui

DISTFILES += \
    .astylerc

unix: CONFIG += link_pkgconfig
unix: PKGCONFIG += libusb-1.0
