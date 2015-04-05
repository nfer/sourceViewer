
#ifndef PROJECTMANAGER_H
#define PROJECTMANAGER_H

#include "stable.h"
#include "StandardItemListView.h"

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

private:
    QPushButton *createButton(const QString &text, const char *member = NULL);

    QLabel             * mCurPath;
    QTreeView          * mDirTreeView;
    QTreeView          * mCurDirTreeView;
    QFileSystemModel   * mDirTreeModel;
    QFileSystemModel   * mCurDirTreeModel;
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
};

#endif
