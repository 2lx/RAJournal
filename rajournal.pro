TARGET = RAJournal
VERSION = 0.1
TEMPLATE = app

QT += core gui sql svg network phonon
CONFIG += qt thread debug_and_release

PRECOMPILED_HEADER = src/stdafx.h
DESTDIR = ./
OBJECTS_DIR = obj
MOC_DIR = tmp
UI_DIR = tmp
RCC_DIR = tmp

# DEFINES += QT_XML_LIB QT_NETWORK_LIB
win{
LIBS += -L"d:/Dev/lib/qwt-6.0.1/" -lqwt
}
unix{
LIBS += -L"/home/aim/Dev/extlibs/lib/qwt6" -lqwt
}
INCLUDEPATH += ./ \
	./tmp/ \
	./src \
	./database \
	/home/aim/Dev/extlibs/include/qwt6

include(rajournal.pri)
