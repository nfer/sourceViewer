#include "Utils.h"

bool Utils::writeString(QString path, QString key, QString value)
{
    if(path.isEmpty() || key.isEmpty())
    {
        return false;
    }

    QSettings *config = new QSettings(path, QSettings::IniFormat);
    config->beginGroup("config");
    config->setValue(key, value);
    config->endGroup();

    return true;
}

bool Utils::readString(QString path, QString key, QString &value)
{
    if(path.isEmpty() || key.isEmpty()){
        return false;
    }

    QSettings *config = new QSettings(path, QSettings::IniFormat);
    value = config->value(QString("config/") + key).toString();
    return true;
}
