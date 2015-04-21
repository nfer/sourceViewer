
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

Utils::Utils():
    mProjectConfigFile(),
    mProjectConfig(NULL)
{
    mDefaultConfigFile = getSVDocumentsLocation() +"/" + SV_PROGRAM_NAME + ".config";
    mDefaultConfig = new QSettings(mDefaultConfigFile, QSettings::IniFormat);
}

Utils::~Utils()
{
    delete mDefaultConfig;
    delete mProjectConfig;
}

void Utils::setProjectConfigFile(QString & file)
{
    if (NULL != mProjectConfig)
        delete mProjectConfig;

    mProjectConfigFile = file;
    mProjectConfig = new QSettings(mProjectConfigFile, QSettings::IniFormat);
}

bool Utils::writeInt(QString key, int value)
{
    if(key.isEmpty())
    {
        return false;
    }

    if (NULL == mProjectConfig){
        qWarning() << "Please setProjectConfigFile() first!";
        return false;
    }

    mProjectConfig->beginGroup("config");
    mProjectConfig->setValue(key, value);
    mProjectConfig->endGroup();

    return true;
}

int  Utils::readInt(QString key)
{
    if(key.isEmpty()){
        return false;
    }

    if (NULL == mProjectConfig){
        qWarning() << "Please setProjectConfigFile() first!";
        return false;
    }

    QString value = mProjectConfig->value(QString("config/") + key).toString();

    if (value.length() == 0){
        // not found in key in config, try program config file
        value = mDefaultConfig->value(QString("config/") + key).toString();
    }

    return value.toInt();
}

bool Utils::writeString(QString key, QString value)
{
    if(key.isEmpty())
    {
        return false;
    }

    if (NULL == mProjectConfig){
        qWarning() << "Please setProjectConfigFile() first!";
        return false;
    }

    mProjectConfig->beginGroup("config");
    mProjectConfig->setValue(key, value);
    mProjectConfig->endGroup();

    return true;
}

bool Utils::readString(QString key, QString &value)
{
    if(key.isEmpty()){
        return false;
    }

    if (NULL == mProjectConfig){
        qWarning() << "Please setProjectConfigFile() first!";
        return false;
    }

    value = mProjectConfig->value(QString("config/") + key).toString();

    if (value.length() == 0){
        // not found in key in config, try program config file
        value = mDefaultConfig->value(QString("config/") + key).toString();
    }

    return true;
}

bool Utils::writeStringList(QString key, QStringList value)
{
    if(key.isEmpty())
    {
        return false;
    }

    if (NULL == mProjectConfig){
        qWarning() << "Please setProjectConfigFile() first!";
        return false;
    }

    mProjectConfig->beginGroup("config");
    mProjectConfig->setValue(key, value);
    mProjectConfig->endGroup();

    return true;
}

bool Utils::readStringList(QString key, QStringList &value)
{
    if(key.isEmpty()){
        return false;
    }

    if (NULL == mProjectConfig){
        qWarning() << "Please setProjectConfigFile() first!";
        return false;
    }

    value = mProjectConfig->value(QString("config/") + key).toStringList();

    if (value.length() == 0){
        // not found in key in config, try default config file
        value = mDefaultConfig->value(QString("config/") + key).toStringList();
    }

    return true;
}

bool Utils::isIgnoredFile(const QString & file, const QStringList & ignoreFileList)
{
    return ignoreFileList.contains(file);
}

bool Utils::IsIgnoreSuffix(const QString & file, const QStringList & ignoreSuffixList)
{
    for (int i=0; i<ignoreSuffixList.size(); i++){
        QString ignoreSuffix = ignoreSuffixList.at(i);
        ignoreSuffix.remove('*');
        if (file.endsWith(ignoreSuffix)){
            return true;
        }
    }

    return false;
}

bool Utils::isIgnoredFolder(const QString &file, const QStringList & ignoreFolderList)
{
    return ignoreFolderList.contains(file);
}
