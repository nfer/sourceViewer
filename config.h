#ifndef SV_CONFIG_H
#define SV_CONFIG_H

#include "stable.h"

#define SV_PROGRAM_NAME "Source Viewer"

inline QString getSVDocumentsLocation()
{
    QStringList list = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    QString location = list.at(0) + "/" + SV_PROGRAM_NAME;

    QDir dir = QDir(location);
    if (!dir.exists()){
        dir.mkdir(location);
        qDebug() << "create Documents location: " << location;
    }

    return location;
}

inline QString getSVProjectsLocation()
{
    QString location = getSVDocumentsLocation() + "/Projects";

    QDir dir = QDir(location);
    if (!dir.exists()){
        dir.mkdir(location);
        qDebug() << "create Projects workspace: " << location;
    }

    return location;
}

#endif // SV_CONFIG_H
