#ifndef FILELISTFRAME_H
#define FILELISTFRAME_H

#include "stable.h"

class FileListFrame : public QDockWidget
{
    Q_OBJECT

public:
    explicit FileListFrame(const QString &dockName, QWidget *parent = 0, Qt::WindowFlags flags = 0);
};

#endif //FILELISTFRAME_H
