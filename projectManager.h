
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
    AddFilesDialog(const QString &storePath, QWidget *parent = 0);

private slots:
    void showFolder();
    void openFolderOfItem(int row, int column);

private:
    QPushButton *createButton(const QString &text, const char *member = NULL);
    void showFiles(const QStringList &files);

    QLabel             * mCurPathLabel;
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

    QString       mCurrentPath;
};

#endif
