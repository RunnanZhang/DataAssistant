#-------------------------------------------------
#
# Project created by QtCreator 2016-09-17T13:39:43
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DataAssistant
TEMPLATE = app


SOURCES += main.cpp\
        dataassistant.cpp \
    customtoolbutton.cpp \
    godefaultbutton.cpp

HEADERS  += dataassistant.h \
    customtoolbutton.h \
    godefaultbutton.h

FORMS    += dataassistant.ui

RESOURCES += \
    dataassistant.qrc

RC_ICONS = images/bird.ico
