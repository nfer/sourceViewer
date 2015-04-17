#ifndef FILELISTFRAME_H
#define FILELISTFRAME_H

#include "stable.h"

class FileListFrame : public QDockWidget
{
    Q_OBJECT

public:
    explicit FileListFrame(const QString &dockName, QWidget *parent = 0, Qt::WindowFlags flags = 0);
    void setListFile(const QString &fileName, const QString & srcRootPath = "");

signals:
    void onFileSelected(const QString &);

private slots:
    void dockFileSelected(const QString &fileName);

private:
    QFrame *mDock;
};

#endif //FILELISTFRAME_H
