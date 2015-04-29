
#include "stable.h"
#include "Utils.h"

static Utils     * s_Utils;
static QString     s_AppName;
static QString     s_DocumentPath;
static QString     s_ProjectPath;

Utils * Utils::enstance(){
    if (NULL == s_Utils){
        s_Utils = new Utils();
    }
    return s_Utils;
}

QString Utils::getAppName(){
    if (!s_AppName.isEmpty())
        return s_AppName;

    s_AppName = QGuiApplication::applicationDisplayName();
    if (s_AppName.isEmpty())
        return SV_PROGRAM_NAME;
    else
        return s_AppName;
}

QString Utils::getDocumentPath(){
    if (!s_DocumentPath.isEmpty())
        return s_DocumentPath;

    QStringList list = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);
    s_DocumentPath = list.at(0) + "/" + Utils::getAppName();

    QDir dir = QDir(s_DocumentPath);
    if (!dir.exists()){
        dir.mkdir(s_DocumentPath);
        qDebug() << "create Documents location: " << s_DocumentPath;
    }

    return s_DocumentPath;
}

QString Utils::getProjectPath(){
    if (!s_ProjectPath.isEmpty())
        return s_ProjectPath;

    s_ProjectPath = getDocumentPath() + "/Projects";

    QDir dir = QDir(s_ProjectPath);
    if (!dir.exists()){
        dir.mkdir(s_ProjectPath);
        qDebug() << "create Projects workspace: " << s_ProjectPath;
    }

    return s_ProjectPath;
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
    QString projName = config->value(QString(SETTINGS_CONFIG) + "/" + PROJNAME).toString();
    QString storePath = config->value(QString(SETTINGS_CONFIG) + "/" + PROJSTOREPATH).toString();

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
    QString projNameListKey = QString(PROJECT_CONFIG) + "/" + PROJECTNAMELIST;
    mProjNameList = mDefaultConfig->value(projNameListKey).toStringList();
}

Utils::~Utils()
{
    delete mDefaultConfig;
    delete mProjectConfig;
}

void Utils::setCurrentProject(QString & name, QString & storePath){
    if (name.isEmpty() || storePath.isEmpty()){
        mProjName.clear();
        mProjStorePath.clear();
        delete mProjectConfig;
        mProjectConfig = NULL;
    }
    else{
        mProjName = name;
        mProjStorePath = storePath;
        mProjectConfig = new QSettings(getProjectConfigFile(), QSettings::IniFormat);
        // FIXME: no need write config here
        writeString(PROJNAME, mProjName);
        writeString(PROJSTOREPATH, mProjStorePath);
    }
}

bool Utils::writeInt(QString key, int value)
{
    if(key.isEmpty()){
        return false;
    }

    if (NULL == mProjectConfig){
        qWarning() << "Please set project Name and StorePath first!";
        return false;
    }

    mProjectConfig->beginGroup(SETTINGS_CONFIG);
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

    QString stringListKey = QString(SETTINGS_CONFIG) + "/" +key;
    QString value = mProjectConfig->value(stringListKey).toString();

    if (value.isEmpty()){
        // not found in key in config, try program config file
        value = mDefaultConfig->value(stringListKey).toString();
    }

    return value.toInt();
}

bool Utils::writeString(QString key, QString & value)
{
    if(key.isEmpty()){
        return false;
    }

    if (NULL == mProjectConfig){
        qWarning() << "Please set project Name and StorePath first!";
        return false;
    }

    mProjectConfig->beginGroup(SETTINGS_CONFIG);
    if(value.isEmpty())
        mProjectConfig->remove(key);
    else
        mProjectConfig->setValue(key, value);
    mProjectConfig->endGroup();

    return true;
}

QString Utils::readString(QString key)
{
    if(key.isEmpty()){
        return QString::null;
    }

    if (NULL == mProjectConfig){
        qWarning() << "Please set project Name and StorePath first!";
        return QString::null;
    }

    QString stringListKey = QString(SETTINGS_CONFIG) + "/" + key;
    QString value = mProjectConfig->value(stringListKey).toString();

    if (value.isEmpty()){
        // not found in key in config, try program config file
        value = mDefaultConfig->value(stringListKey).toString();
    }

    return value;
}

bool Utils::writeStringList(QString key, QStringList & value)
{
    if(key.isEmpty()){
        return false;
    }

    if (NULL == mProjectConfig){
        qWarning() << "Please set project Name and StorePath first!";
        return false;
    }

    mProjectConfig->beginGroup(SETTINGS_CONFIG);
    if(value.isEmpty())
        mProjectConfig->remove(key);
    else
        mProjectConfig->setValue(key, value);
    mProjectConfig->endGroup();

    return true;
}

QStringList Utils::readStringList(QString key)
{
    if(key.isEmpty()){
        return QStringList();
    }

    if (NULL == mProjectConfig){
        qWarning() << "Please set project Name and StorePath first!";
        return QStringList();
    }

    QString stringListKey = QString(SETTINGS_CONFIG) + "/" + key;
    QStringList value = mProjectConfig->value(stringListKey).toStringList();

    if (value.isEmpty()){
        // not found in key in config, try default config file
        value = mDefaultConfig->value(stringListKey).toStringList();
    }

    return value;
}

bool Utils::removeConfig(QString key)
{
    if(key.isEmpty()){
        return false;
    }

    if (NULL == mProjectConfig){
        qWarning() << "Please set project Name and StorePath first!";
        return false;
    }

    mProjectConfig->beginGroup(SETTINGS_CONFIG);
    mProjectConfig->remove(key);
    mProjectConfig->endGroup();

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
    QString projStorePathKey = QString(PROJECT_CONFIG) + "/" + name;
    return mDefaultConfig->value(projStorePathKey).toString();

}

void Utils::addProject(const QString & name, const QString & storePath)
{
    mProjNameList += name;

    mDefaultConfig->beginGroup(PROJECT_CONFIG);
    mDefaultConfig->setValue(PROJECTNAMELIST, mProjNameList);
    mDefaultConfig->setValue(name, storePath);
    mDefaultConfig->endGroup();
}

void Utils::removeProject(const QString & name)
{
    mProjNameList.removeOne(name);

    mDefaultConfig->beginGroup(PROJECT_CONFIG);
    mDefaultConfig->setValue(PROJECTNAMELIST, mProjNameList);
    mDefaultConfig->remove(name);
    mDefaultConfig->endGroup();
}

bool Utils::isProjectExist(const QString & name)
{
    return mProjNameList.contains(name);
}
