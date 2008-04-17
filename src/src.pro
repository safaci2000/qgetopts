TEMPLATE = lib
CONFIG += warn_on \
	  thread \
          qt dll

QT = core

TARGET = qgetopts
DESTDIR = ../bin
OBJECTS_DIR = ../obj
MOC_DIR = $$OBJECTS_DIR

SOURCES += qoptions.cpp \
 qoption.cpp

HEADERS += qoptions.h \
 qoption.h
