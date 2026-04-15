QT += widgets

TARGET = client
TEMPLATE = app

HEADERS += src/Simulator.h \
        src/MainWindow.h \
        src/WorldWidget.h

# List the source files
SOURCES += src/main.cpp \
        src/Simulator.cpp \
        src/MainWindow.cpp \
        src/WorldWidget.cpp

# Find headers here
INCLUDEPATH += src

!exists(src/main.cpp) {
    error("No main.cpp file found")
}
