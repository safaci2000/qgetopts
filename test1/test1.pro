TEMPLATE = app
TARGET = test1

DESTDIR = ../bin
OBJECTS_DIR = ../obj/$$TARGET
MOC_DIR = $$OBJECTS_DIR

LIBS += -lqgetopts -L../bin
INCLUDEPATH += ../src

SOURCES += main.cpp

DEPENDPATH += ../src