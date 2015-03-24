TEMPLATE   = app

QT        += widgets
TARGET     = sourceViewer

HEADERS   += mainwindow.h \
             config.h \
             stable.h \
             windowswatch.h \
             filelistframe.h

SOURCES   += main.cpp\
             mainwindow.cpp \
             windowswatch.cpp \
             filelistframe.cpp

RESOURCES  = sourceViewer.qrc

PRECOMPILED_HEADER = stable.h
