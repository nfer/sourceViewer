#ifndef UTILS_H
#define UTILS_H

#include "stable.h"

class Utils{
public:
    static Utils * enstance();
    bool writeString(QString path, QString key, QString value);
    bool readString(QString path, QString key, QString &value);
    bool writeStringList(QString path, QString key, QStringList value);
    bool readStringList(QString path, QString key, QStringList &value);

    bool isIgnoredFile(const QString &file, const QStringList & ignoreFileList);
    bool IsIgnoreSuffix(const QString & file, const QStringList & ignoreSuffixList);
    bool isIgnoredFolder(const QString & file, const QStringList & ignoreFolderList);

private:
    Utils();
};

#endif // UTILS_H
