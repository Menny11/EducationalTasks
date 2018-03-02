TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS +=-g -Wall -Wextra -Wpedantic -std=c++14 -pthread

SOURCES += \
    imgResizeThreads.cpp \
    imgResizeMain.cpp \
    imgResizeAsync.cpp

HEADERS += \
    imgResizeThreads.h \
    imgResizeAsync.h \
    exectimerclass.h

LIBS += \
    -pthread \
    -L/usr/local/lib -lopencv_core -lopencv_stitching -lopencv_imgproc\
    -lopencv_photo -lopencv_highgui -lopencv_imgcodecs

INCLUDEPATH += /usr/local/include/opencv


