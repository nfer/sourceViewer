
#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include "stable.h"
#include "FileSystemModel.h"

class NewProjectDialog : public QDialog
{
    Q_OBJECT

public:
    NewProjectDialog(QWidget *parent = 0);
    QString getProjName(){
        return mNameEdit->text();
    }
    QString getProjStorePath(){
        return mProjStorePathEdit->text();
    }
    QString getSrcRootPath(){
        return mSrcRootPathEdit->text();
    }

private slots:
    void browseProjStorePath();
    void browseSrcRootPath();
    void accept();
    void onNameChanged(const QString & text);
private:
    QPushButton *createButton(const QString &text, const char *member);

    QLabel      * mNameLabel;
    QLineEdit   * mNameEdit;
    QLabel      * mProjStorePathLabel;
    QLineEdit   * mProjStorePathEdit;
    QPushButton * mProjStorePathButton;
    QLabel      * mSrcRootPathLabel;
    QLineEdit   * mSrcRootPathEdit;
    QPushButton * mSrcRootPathButton;
    QPushButton * mCancelButton;
    QPushButton * mNextButton;
};

class AddFilesDialog : public QDialog
{
    Q_OBJECT

public:
    AddFilesDialog(const QString & projName, const QString & projStorePath,
        const QString & srcRootPath, QWidget *parent = 0);

private slots:
    void accept();
    void showFolder();
    void curDirDoubleClicked(int row, int column);
    void fileListDoubleClicked(const QModelIndex & index);
    void dirSelected(const QModelIndex & current, const QModelIndex &previous);
    void sortCurDir(int index);
    void curPathInput();
    void addFile();
    void addAll();
    void addTree();
    void removeFile();
    void removeAll();
    void removeTree();
    void curDirItemChanged(QTableWidgetItem * current, QTableWidgetItem * previous);

private:
    QPushButton *createButton(const QString &text, const char *member = NULL);
    void showFiles(const QStringList &files);
    bool eventFilter(QObject*obj,QEvent*event);
    void updateTreeView(const QString & path);
    void cdDirOrAddFileToList(QString itemText);
    void searchFiles(QString path, QStringList& fileList, bool isRecursively = false);
    void updateFileListTitle();

    QLineEdit          * mCurPathEdit;
    QTreeView          * mDirTreeView;
    QTableWidget       * mCurDirTableWidget;
    FileSystemModel    * mDirTreeModel;
    FileSystemModel    * mCurDirTreeModel;
    QLabel             * mFileListTitle;
    QListView          * mFileListView;
    QStandardItemModel * mFileListModel;

    QPushButton * mAddFileButton;
    QPushButton * mAddAllButton;
    QPushButton * mAddTreeButton;
    QPushButton * mRemoveFileButton;
    QPushButton * mRemoveAllButton;
    QPushButton * mRemoveTreeButton;

    QPushButton * mOKButton;

    QString       mProjName;
    QString       mProjStorePath;
    QString       mCurrentPath;

    Qt::SortOrder mSortOrder;
};

class AddAllDialog : public QDialog
{
    Q_OBJECT

public:
    AddAllDialog(QWidget *parent = 0);
    bool isRecursively(){
        return (mRecursively->checkState() == Qt::Checked);
    }

private:
    QPushButton *createButton(const QString &text, const char *member = NULL);

    QCheckBox   * mRecursively;
    QPushButton * mOKButton;
    QPushButton * mCancelButton;
};

#endif
