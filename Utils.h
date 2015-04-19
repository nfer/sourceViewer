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

private:
    Utils();
};

#endif // UTILS_H
