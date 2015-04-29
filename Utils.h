#ifndef UTILS_H
#define UTILS_H

#include "stable.h"
#include "config.h"

class Utils
{
public:
    static Utils * enstance();

    static QString getAppName();
    static QString getDocumentPath();
    static QString getProjectPath();

    static bool isFullFilePath(QString & path);
    static bool isProjectFile(QString & fileName);
    static QRegExp trRegExp(QString & str,
                    Qt::CaseSensitivity cs = Qt::CaseSensitive,
                    QRegExp::PatternSyntax syntax = QRegExp::RegExp);

    void setCurrentProject(QString & name, QString & storePath);

    int         readInt(QString key);
    QString     readString(QString key);
    QStringList readStringList(QString key);
    bool writeInt(QString key, int value);
    bool writeString(QString key, QString & value);
    bool writeStringList(QString key, QStringList & value);

    bool isIgnoredFile(const QString &file, const QStringList & ignoreFileList);
    bool IsIgnoreSuffix(const QString & file, const QStringList & ignoreSuffixList);
    bool isIgnoredFolder(const QString & file, const QStringList & ignoreFolderList);

    QStringList getProjNameList(){
        return mProjNameList;
    }
    QString getProjStorePath(const QString & name);
    void addProject(const QString & name, const QString & storePath);
    void removeProject(const QString & name);
    bool isProjectExist(const QString & name);

    QString getProjFileListFile(){
        return mProjStorePath + "/" + mProjName + FILELIST_SUFFIX;
    }

    QString getDefaultConfigFile(){
        return getDocumentPath() +"/" + Utils::getAppName() + CONFIG_SUFFIX;
    }

    QString getProjectConfigFile(){
        return mProjStorePath + "/" + mProjName + PROJECT_SUFFIX;
    }

private:
    Utils();
    ~Utils();

    QString     mProjName;
    QString     mProjStorePath;

    QStringList mProjNameList;

    QSettings * mDefaultConfig;
    QSettings * mProjectConfig;
};

#endif // UTILS_H
