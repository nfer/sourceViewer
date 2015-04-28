#ifndef UTILS_H
#define UTILS_H

#include "stable.h"
#include "config.h"

#define PROJNAME            "PROJNAME"
#define PROJSTOREPATH       "PROJSTOREPATH"
#define SRCROOTPATH         "SRCROOTPATH"
#define IGNOREFOLDERLIST    "IGNOREFOLDERLIST"
#define IGNOREFILELIST      "IGNOREFILELIST"
#define LASTOPENEDFILES     "LASTOPENEDFILES"

#define PROJECT_SUFFIX      ".project"

class Utils
{
public:
    static Utils * enstance();
    static bool isFullFilePath(QString & path);
    static bool isProjectFile(QString & fileName);
    static QRegExp trRegExp(QString & str,
                    Qt::CaseSensitivity cs = Qt::CaseSensitive,
                    QRegExp::PatternSyntax syntax = QRegExp::RegExp);

    void setCurrentProject(QString & name, QString & storePath);

    bool writeInt(QString key, int value);
    int  readInt(QString key);
    bool writeString(QString key, QString value);
    bool readString(QString key, QString &value);
    bool writeStringList(QString key, QStringList value);
    bool readStringList(QString key, QStringList &value);

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
        return mProjStorePath + "/" + mProjName + ".filelist";
    }

    QString getDefaultConfigFile(){
        return getSVDocumentsLocation() +"/" + SV_PROGRAM_NAME + ".config";
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
