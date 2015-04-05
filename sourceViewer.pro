TEMPLATE   = app

QT        += widgets
TARGET     = sourceViewer

HEADERS   += mainwindow.h \
             config.h \
             stable.h \
             windowswatch.h \
             filelistframe.h \
             projectManager.h \
             StandardItemListView.h \
             FileSystemModel.h

SOURCES   += main.cpp\
             mainwindow.cpp \
             windowswatch.cpp \
             filelistframe.cpp \
             projectManager.cpp \
             StandardItemListView.cpp \
             FileSystemModel.cpp

RESOURCES  = sourceViewer.qrc

PRECOMPILED_HEADER = stable.h
