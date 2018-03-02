TEMPLATE = app
CONFIG += console c++14
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS +=-g -Wall -Wextra -Wpedantic -std=c++14 -pthread

SOURCES += \
    engineMain.cpp \
    imgthreadpool.cpp

HEADERS += \
    imgthreadpool.h \
    exectimerclass.h

LIBS += \
    -pthread -lboost_system -lboost_filesystem \
    -L/usr/local/lib -lopencv_core -lopencv_imgproc\
    -lopencv_highgui -lopencv_imgcodecs

INCLUDEPATH += \
    /usr/local/include/opencv
