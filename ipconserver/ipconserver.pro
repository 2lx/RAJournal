TARGET = IPConServer
VERSION = 0.1
TEMPLATE = app

QT += core gui sql network
CONFIG += qt thread debug_and_release

PRECOMPILED_HEADER = stdafx.h
DESTDIR = ./
OBJECTS_DIR = tmp
MOC_DIR = tmp
UI_DIR = tmp
RCC_DIR = tmp

# DEFINES += QT_XML_LIB QT_NETWORK_LIB
INCLUDEPATH += ./ \
	./tmp \
	../database

include(ipconserver.pri)
