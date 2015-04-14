
#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include "stable.h"
#include "StandardItemListView.h"
#include "FileSystemModel.h"

class NewProjectDialog : public QDialog
{
    Q_OBJECT

public:
    NewProjectDialog(QWidget *parent = 0);
    QString getProjectName(){
        return mNameEdit->text();
    }
    QString getStorePath(){
        return mStorePathEdit->text();
    }

private slots:
    void browse();
    void accept();
    void onNameChanged(const QString & text);
private:
    QPushButton *createButton(const QString &text, const char *member);

    QLineEdit   * mNameEdit;
    QLineEdit   * mStorePathEdit;
    QLabel      * mNameLabel;
    QLabel      * mStorePathLabel;
    QPushButton * mOKButton;
    QPushButton * mCancelButton;
    QPushButton * mBrowseButton;
};

class AddFilesDialog : public QDialog
{
    Q_OBJECT

public:
    AddFilesDialog(const QString &projectName, const QString &storePath, QWidget *parent = 0);

private slots:
    void accept();
    void showFolder();
    void curDirCellActivated(int row, int column);
    void dirSelected(const QModelIndex & current, const QModelIndex &previous);
    void sortCurDir(int index);
    void curPathInput();
    void cdDirOrAddFile();
    void addAll();
    void addTree();
    void curDirItemChanged(QTableWidgetItem * current, QTableWidgetItem * previous);

private:
    QPushButton *createButton(const QString &text, const char *member = NULL);
    void showFiles(const QStringList &files);
    bool eventFilter(QObject*obj,QEvent*event);
    void updateTreeView(const QString & path);
    void cdDirOrAddFileToList(QString itemText);
    void searchFiles(QString path, QStringList& fileList, bool isRecursively = false);

    QLineEdit          * mCurPathEdit;
    QTreeView          * mDirTreeView;
    QTableWidget       * mCurDirTableWidget;
    FileSystemModel    * mDirTreeModel;
    FileSystemModel    * mCurDirTreeModel;
    QLabel             * mFileListTitle;
    QListView          * mFileListView;
    QStandardItemModel * mFileListModel;

    QPushButton * mAddButton;
    QPushButton * mAddAllButton;
    QPushButton * mAddTreeButton;
    QPushButton * mRemoveButton;
    QPushButton * mRemoveAllButton;
    QPushButton * mRemoveTreeButton;

    QPushButton * mOKButton;
    QPushButton * mCancelButton;

    QString       mProjectName;
    QString       mStorePath;
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
