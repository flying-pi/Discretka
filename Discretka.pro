#-------------------------------------------------
#
# Project created by QtCreator 2016-05-13T12:42:56
#
#-------------------------------------------------

QT       += core printsupport gui
QMAKE_CXXFLAGS += -std=c++14 -stdlib=libc++

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Discretka
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    field.cpp \
    graph/graphvertex.cpp \
    graph/graph.cpp \
    optionmenurow.cpp \
    logic/printutil.cpp \
    nameeditmenu.cpp \
    logic/discretcaprinter.cpp \
    preview.cpp

HEADERS  += mainwindow.h \
    field.h \
    graph/graphvertex.h \
    const.h \
    graph/graph.h \
    optionmenurow.h \
    logic/printutil.h \
    nameeditmenu.h \
    logic/discretcaprinter.h \
    preview.h

FORMS    += mainwindow.ui \
    field.ui \
    optionmenurow.ui \
    nameeditmenu.ui \
    preview.ui

CONFIG += mobility

MOBILITY =


#QMAKE_CXXFLAGS_CXX11 = -std=c++14 -stdlib=libc++



