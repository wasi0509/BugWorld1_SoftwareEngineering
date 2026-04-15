QT += core gui widgets testlib

TEMPLATE = app
CONFIG += testcase

TARGET = test_simulator

SOURCES += \
    ../../tests/test_simulator.cpp \
    ../../src/Simulator.cpp \
    ../../src/WorldWidget.cpp\
    ../../src/MainWindow.cpp

HEADERS += \
    ../../src/Simulator.h  \
    ../../src/MainWindow.h \
    ../../src/WorldWidget.h



