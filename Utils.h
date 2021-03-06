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

    void saveLayout(QMainWindow * window);
    void loadLayout(QMainWindow * window);

    void setCurrentProject(QString & name, QString & storePath);
    QString getCurProjName();

    int         readInt(QString key);
    QString     readString(QString key);
    QStringList readStringList(QString key);
    bool writeInt(QString key, int value);
    bool writeString(QString key, QString & value);
    bool writeStringList(QString key, QStringList & value);
    bool removeConfig(QString key);

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
        if (mProjStorePath.isEmpty() || mProjName.isEmpty())
            return QString::null;
        else
            return mProjStorePath + "/" + mProjName + FILELIST_SUFFIX;
    }

private:
    Utils();
    ~Utils();

    QString getDefaultLayoutFile(){
        return getDocumentPath() +"/" + Utils::getAppName() + LAYOUT_SUFFIX;
    }

    QString getDefaultConfigFile(){
        return getDocumentPath() +"/" + Utils::getAppName() + CONFIG_SUFFIX;
    }

    QString getProjectConfigFile(){
        return mProjStorePath + "/" + mProjName + PROJECT_SUFFIX;
    }

    QString     mProjName;
    QString     mProjStorePath;

    QStringList mProjNameList;

    QSettings * mDefaultConfig;
    QSettings * mProjectConfig;
};

#endif // UTILS_H
