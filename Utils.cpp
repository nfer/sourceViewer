
#include "stable.h"
#include "Utils.h"

static Utils * utils;

Utils * Utils::enstance(){
    if (NULL == utils){
        utils = new Utils();
    }
    return utils;
}

bool Utils::isFullFilePath(QString & path)
{
#if defined (Q_OS_WIN32)
    return path.contains(QRegExp("^[a-zA-Z]:"));
#else
    return path.startsWith("/");
#endif
}

bool Utils::isProjectFile(QString & fileName)
{
    if (!fileName.endsWith(PROJECT_SUFFIX)){
        qWarning() << QString("file(%1) suffix is not a PROJECT_SUFFIX(%2).")
                        .arg(fileName).arg(PROJECT_SUFFIX);
        return false;
    }

    QFileInfo info = QFileInfo(fileName);
    if (!info.exists()){
        qWarning() << QString("file(%1) not exist.").arg(fileName);
        return false;
    }

    QString name = info.baseName();
    QString path = info.path();

    QSettings * config = new QSettings(fileName, QSettings::IniFormat);
    QString projName = config->value(QString("config/") + PROJNAME).toString();
    QString storePath = config->value(QString("config/") + PROJSTOREPATH).toString();

    if (name != projName || path != storePath){
        qWarning() << QString("file name is %1, but project name is %2.")
                    .arg(name).arg(projName);
        qWarning() << QString("file path is %1, but project storePath is %2.")
                    .arg(path).arg(storePath);
        return false;
    }

    return true;
}

QRegExp Utils::trRegExp(QString & str, Qt::CaseSensitivity cs, QRegExp::PatternSyntax syntax)
{
    QString newStr;
    for (int i = 0; i < str.length(); i++)
    {
        newStr.append(str.at(i));
        if (i != str.length() - 1)
            newStr.append('*');
    }

    return QRegExp(newStr, cs, syntax);
}

Utils::Utils():
    mProjectConfig(NULL)
{
    mDefaultConfig = new QSettings(getDefaultConfigFile(), QSettings::IniFormat);
    mProjNameList = mDefaultConfig->value("project/PROJECTNAMELIST").toStringList();
}

Utils::~Utils()
{
    delete mDefaultConfig;
    delete mProjectConfig;
}

void Utils::setCurrentProject(QString & name, QString & storePath){
    mProjName = name;
    mProjStorePath = storePath;

    mProjectConfig = new QSettings(getProjectConfigFile(), QSettings::IniFormat);

    writeString(PROJNAME, mProjName);
    writeString(PROJSTOREPATH, mProjStorePath);
}

bool Utils::writeInt(QString key, int value)
{
    if(key.isEmpty())
    {
        return false;
    }

    if (NULL == mProjectConfig){
        qWarning() << "Please set project Name and StorePath first!";
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
        qWarning() << "Please set project Name and StorePath first!";
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
        qWarning() << "Please set project Name and StorePath first!";
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
        qWarning() << "Please set project Name and StorePath first!";
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
        qWarning() << "Please set project Name and StorePath first!";
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
        qWarning() << "Please set project Name and StorePath first!";
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

QString Utils::getProjStorePath(const QString & name)
{
    return mDefaultConfig->value(QString("project/") + name).toString();

}

void Utils::addProject(const QString & name, const QString & storePath)
{
    mProjNameList += name;

    mDefaultConfig->beginGroup("project");
    mDefaultConfig->setValue("PROJECTNAMELIST", mProjNameList);
    mDefaultConfig->setValue(name, storePath);
    mDefaultConfig->endGroup();
}

void Utils::removeProject(const QString & name)
{
    mProjNameList.removeOne(name);

    mDefaultConfig->beginGroup("project");
    mDefaultConfig->setValue("PROJECTNAMELIST", mProjNameList);
    mDefaultConfig->remove(name);
    mDefaultConfig->endGroup();
}

bool Utils::isProjectExist(const QString & name)
{
    return mProjNameList.contains(name);
}
