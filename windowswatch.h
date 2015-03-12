#ifndef WINDOWSWATCH_H
#define WINDOWSWATCH_H

#include "stable.h"

class WindowSwatch : public QDockWidget
{
    Q_OBJECT

public:
    explicit WindowSwatch(const QString &dockName, QWidget *parent = 0, Qt::WindowFlags flags = 0);
};

#endif //WINDOWSWATCH_H
