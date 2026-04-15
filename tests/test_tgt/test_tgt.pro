QT += core gui widgets testlib

TEMPLATE = app
CONFIG += testcase

TARGET = test_together

SOURCES += \
    ../../tests/test_together.cpp \
    ../../src/Simulator.cpp \
    ../../src/MainWindow.cpp \
    ../../src/WorldWidget.cpp

HEADERS += \
    ../../src/Simulator.h  \
    ../../src/MainWindow.h \
    ../../src/WorldWidget.h



