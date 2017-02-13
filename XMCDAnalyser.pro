#-------------------------------------------------
#
# Project created by QtCreator 2016-10-08T13:19:31
#
#-------------------------------------------------

QT       += core gui charts

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = XMCDAnalyser
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
        extfsm.cpp \
    fileloader.cpp

HEADERS  += mainwindow.h \
        extfsm.h \
    fileloader.h

FORMS    += mainwindow.ui

RC_FILE = XMCDAnalyser.rc
