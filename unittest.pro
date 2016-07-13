TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

QMAKE_CXXFLAGS = -std=c++03 -Wall -Wextra -pedantic -Werror

LIBS += -lgmock -lpthread
LIBS += -lgmock_main

INCLUDEPATH += src/

SOURCES += \
    src/DirectMemory.cpp \
    src/AdcMeasurement.cpp \
    src/TiAdc.cpp \
    src/AdcMeasurement.Test.cpp \
    src/DirectMemory.Test.cpp \
    src/Adc.Dummy.cpp \
    src/TiAdc.Test.cpp \
    src/Trace.Test.cpp \
    src/Trace.cpp \
    src/lightsequence/Up.cpp \
    src/lightsequence/Up.Test.cpp \
    src/Led.Dummy.cpp

HEADERS += \
    src/Adc.h \
    src/MemoryMap.h \
    src/DirectMemory.h \
    src/AdcMeasurement.h \
    src/TiAdc.h \
    src/MemoryMap.Mock.h \
    src/Adc.Mock.h \
    src/Adc.Dummy.h \
    src/precpp11.h \
    src/Trace.h \
    src/lightsequence/Up.h \
    src/Led.Dummy.h \
    src/Led.h
