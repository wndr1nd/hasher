QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++20
CONFIG += console

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    crc32.cpp \
    main.cpp \
    mainwindow.cpp \
    md5.cpp \
    no_gui.cpp \
    sha1.cpp \
    sha256.cpp

HEADERS += \
    Ihash.h \
    crc32.h \
    mainwindow.h \
    md5.h \
    no_gui.h \
    sha1.h \
    sha256.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
