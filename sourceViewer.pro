TEMPLATE   = app

QT        += widgets
TARGET     = sourceViewer

HEADERS   += mainwindow.h \
             config.h \
             stable.h \
             Common.h \
             DockWidget.h \
             FileManager.h \
             projectManager.h \
             FileSystemModel.h \
             CodeEditor.h \
             Utils.h

SOURCES   += main.cpp\
             mainwindow.cpp \
             DockWidget.cpp \
             FileManager.cpp \
             projectManager.cpp \
             FileSystemModel.cpp \
             CodeEditor.cpp \
             Utils.cpp

RESOURCES  = sourceViewer.qrc

PRECOMPILED_HEADER = stable.h
