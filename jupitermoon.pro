######################################################################
# Automatically generated by qmake (2.01a) zo 17. jul 10:48:48 2011
######################################################################

TEMPLATE = app

DEPENDPATH += .
CONFIG += debug

INCLUDEPATH += . aaplus/
QMAKE_RPATHDIR += aaplus/lib
LIBS += -Laaplus/lib -laaplus

# Input
HEADERS += jupitermoon.h astroDateTime.h
FORMS += jupitermoon.ui
SOURCES += main.cpp jupitermoon.cpp astroDateTime.cpp
QT += widgets
