QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

DESTDIR = ../bin

include($$PWD/GeneralModules/GeneralModules.pri);
INCLUDEPATH += -I $$PWD/GeneralModules

win32-msvc* {
    QMAKE_CXXFLAGS += /source-charset:utf-8 /execution-charset:utf-8
}

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    CardReadWrite.cpp \
    main.cpp \
    mainwindow.cpp \
    KeyAlgorithm.h \
    d3des.h \
    sm4.h

HEADERS += \
    CardReadWrite.h \
    mainwindow.h \
    KeyAlgorithm.cpp \
    d3des.cpp \
    sm4.c

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
