#ifndef WINDOWSWATCH_H
#define WINDOWSWATCH_H

#include "stable.h"

class DockWidget : public QDockWidget
{
    Q_OBJECT

public:
    explicit DockWidget(const QString &dockName, QWidget *parent = 0, Qt::WindowFlags flags = 0);
};

class ProjectDock : public DockWidget
{
    Q_OBJECT

public:
    explicit ProjectDock(const QString &dockName, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    void updateFileList();

signals:
    void onFileSelected(const QString &);

private slots:
    void projectFileSelected(const QString &fileName);
};

#endif //WINDOWSWATCH_H
