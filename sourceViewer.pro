#-------------------------------------------------
#
# Project created by QtCreator 2015-02-04T16:12:27
#
#-------------------------------------------------

QT        += widgets
TARGET     = sourceViewer
TEMPLATE   = app
HEADERS   += mainwindow.h \
             config.h \
             stable.h \
             windowswatch.h
RESOURCES  = sourceViewer.qrc
SOURCES   += main.cpp\
             mainwindow.cpp \
             windowswatch.cpp

PRECOMPILED_HEADER = stable.h
