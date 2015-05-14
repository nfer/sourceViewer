
#include "FileSystemModel.h"

FileSystemModel::FileSystemModel(const QStringList & headerList, QObject *parent)
    : QFileSystemModel(parent),
      mHeaderList(headerList)
{
}

QVariant FileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && mHeaderList.count() > section){
        QString string = static_cast<QString>(mHeaderList.at(section));
        return string;
    }
    else {
        return QFileSystemModel::headerData(section,orientation,role);
    }
}
