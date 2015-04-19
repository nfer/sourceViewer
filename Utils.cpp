
#include "stable.h"
#include "config.h"
#include "Utils.h"

static Utils * utils;

Utils * Utils::enstance(){
    if (NULL == utils){
        utils = new Utils();
    }
    return utils;
}

Utils::Utils()
{
    
}

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

    if (value.length() == 0){
        // not found in key in path config, try program config file
        QString defaultConfigPath = getSVDocumentsLocation() +"/" +
                SV_PROGRAM_NAME + ".config";
        QSettings * defaultConfig = new QSettings(defaultConfigPath,
                                                  QSettings::IniFormat);
        value = defaultConfig->value(QString("config/") + key).toString();
    }

    return true;
}

bool Utils::writeStringList(QString path, QString key, QStringList value)
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

bool Utils::readStringList(QString path, QString key, QStringList &value)
{
    if(path.isEmpty() || key.isEmpty()){
        return false;
    }

    QSettings *config = new QSettings(path, QSettings::IniFormat);
    value = config->value(QString("config/") + key).toStringList();

    if (value.length() == 0){
        // not found in key in path config, try program config file
        QString defaultConfigPath = getSVDocumentsLocation() +"/" +
                SV_PROGRAM_NAME + ".config";
        QSettings * defaultConfig = new QSettings(defaultConfigPath,
                                                  QSettings::IniFormat);
        value = defaultConfig->value(QString("config/") + key).toStringList();
    }

    return true;
}