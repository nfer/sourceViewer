#ifndef UTILS_H
#define UTILS_H

#include "stable.h"

class Utils{
public:
    static bool writeString(QString path, QString key, QString value);
    static bool readString(QString path, QString key, QString &value);
};

#endif // UTILS_H
