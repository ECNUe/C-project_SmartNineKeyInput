QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = SmartNineKeyInput
TEMPLATE = app

SOURCES += src\main.cpp \
           src\mainwindow.cpp \
           src\trie.cpp

HEADERS += src\mainwindow.h \
           src\trie.h

FORMS += src\mainwindow.ui
