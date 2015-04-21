#ifndef UTILS_H
#define UTILS_H

#include "stable.h"

class Utils{
public:
    static Utils * enstance();

    void setProjectConfigFile(QString & file);
    QString getProjectConfigFile(){
        return mProjectConfigFile;
    }

    bool writeInt(QString key, int value);
    int  readInt(QString key);
    bool writeString(QString key, QString value);
    bool readString(QString key, QString &value);
    bool writeStringList(QString key, QStringList value);
    bool readStringList(QString key, QStringList &value);

    bool isIgnoredFile(const QString &file, const QStringList & ignoreFileList);
    bool IsIgnoreSuffix(const QString & file, const QStringList & ignoreSuffixList);
    bool isIgnoredFolder(const QString & file, const QStringList & ignoreFolderList);

    void addProject(const QString & name, const QString & storePath);
    void removeProject(const QString & name);
    bool isProjectExist(const QString & name);

private:
    Utils();
    ~Utils();

    QString     mDefaultConfigFile;
    QString     mProjectConfigFile;
    QStringList mProjNameList;

    QSettings * mDefaultConfig;
    QSettings * mProjectConfig;
};

#endif // UTILS_H
