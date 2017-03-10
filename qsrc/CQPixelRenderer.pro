TEMPLATE = lib

TARGET = CQRenderer

DEPENDPATH += .

QT += widgets

CONFIG += staticlib

QMAKE_CXXFLAGS += -std=c++11

MOC_DIR = .moc

SOURCES += \
CQWidgetRenderer2D.cpp \
CQImageRenderer2D.cpp \
CQRenderer2D.cpp \
CQWidgetPixelRenderer.cpp \
CQImagePixelRenderer.cpp \
CQPixelRenderer.cpp \
CQPath2D.cpp \

HEADERS += \
../qinclude/CQWidgetRenderer2D.h \
../qinclude/CQImageRenderer2D.h \
../qinclude/CQRenderer2D.h \
../qinclude/CQWidgetPixelRenderer.h \
../qinclude/CQImagePixelRenderer.h \
../qinclude/CQPixelRenderer.h \
../qinclude/CQPath2D.h \

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
../../COS/include \
../../CUtil/include \
