#ifndef FILESYSTEMMODEL_H
#define FILESYSTEMMODEL_H

#include "stable.h"

class FileSystemModel : public QFileSystemModel
{
    Q_OBJECT

public:
    FileSystemModel(const QStringList & headerList, QObject *parent = 0);
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

private:
    const QStringList mHeaderList;
};

#endif // FILESYSTEMMODEL_H
