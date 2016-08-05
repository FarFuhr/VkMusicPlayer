#-------------------------------------------------
#
# Project created by QtCreator 2016-08-02T00:42:40
#
#-------------------------------------------------

QT      += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets webenginewidgets network multimedia
QT      -= quick qml

TARGET = QtVkPlayer
TEMPLATE = app


SOURCES += main.cpp\
        loginwidget.cpp \
    mainwidget.cpp \
    playlistwidget.cpp \
    clickablelabel.cpp \
    imagebutton.cpp

HEADERS  += loginwidget.h \
    mainwidget.h \
    playlistmember.h \
    playlistwidget.h \
    clickablelabel.h \
    imagebutton.h

FORMS +=
