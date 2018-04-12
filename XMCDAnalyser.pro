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
    fileloader.cpp \
    calculator.cpp \
    pairwidget.cpp \
    calcwrapper.cpp \
    logger.cpp

HEADERS  += mainwindow.h \
        extfsm.h \
    fileloader.h \
    calculator.h \
    pairwidget.h \
    calcwrapper.h \
    logger.h

FORMS    += mainwindow.ui \
    pairwidget.ui

RC_FILE = XMCDAnalyser.rc
