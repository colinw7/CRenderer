TEMPLATE = lib

TARGET = CQRenderer

DEPENDPATH += .

QT += widgets

CONFIG += staticlib

QMAKE_CXXFLAGS += -std=c++11

MOC_DIR = .moc

SOURCES += \
CQWidgetPixelRenderer.cpp \
CQPixelRenderer.cpp \

HEADERS += \
../qinclude/CQWidgetPixelRenderer.h \
../qinclude/CQPixelRenderer.h \

OBJECTS_DIR = ../obj

DESTDIR = ../lib

INCLUDEPATH += \
../qinclude \
../include \
../../CQUtil/include \
../../CImageLib/include \
../../CFont/include \
../../CFile/include \
../../CMath/include \
../../CStrUtil/include \
../../CUtil/include \
