
#include <QtWidgets>
#include <QFileIconProvider>

#include "projectManager.h"
#include "config.h"

NewProjectDialog::NewProjectDialog(QWidget *parent)
    : QDialog(parent)
{
    mOKButton = createButton(tr("OK"), SLOT(accept()));
    mCancelButton = createButton(tr("Cancel"), SLOT(reject()));
    mBrowseButton = createButton(tr("Browse..."), SLOT(browse()));

    mNameLabel = new QLabel(tr("New Project Name..."));
    mStorePathLabel = new QLabel(tr("Where do you want to store the project data file?"));

    QString defaultName = "Unitled Project";
    mNameEdit = new QLineEdit(defaultName);
    mNameEdit->setFixedWidth(360);
    connect(mNameEdit, SIGNAL(textChanged(const QString & )),
                this, SLOT(onNameChanged(const QString & )));

    QString defaultPath = getSVProjectsLocation() + "/" + defaultName;
    mStorePathEdit = new QLineEdit(defaultPath);
    mStorePathEdit->setFixedWidth(360);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setSpacing(4);
    leftLayout->setAlignment(Qt::AlignTop);
    leftLayout->addWidget(mNameLabel);
    leftLayout->addWidget(mNameEdit);
    leftLayout->addWidget(mStorePathLabel);
    leftLayout->addWidget(mStorePathEdit);
    leftLayout->insertSpacing(2, 12);
    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->setSpacing(12);
    rightLayout->addWidget(mOKButton);
    rightLayout->addWidget(mCancelButton);
    rightLayout->addWidget(mBrowseButton);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    mainLayout->insertSpacing(1, 20);
    mainLayout->setContentsMargins(12, 10, 12, 40);
    setLayout(mainLayout);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setWindowTitle(tr("New Project"));
}

void NewProjectDialog::browse()
{
    QString directory = QFileDialog::getExistingDirectory(this,
                                   tr("Browse Files"),
                                   mStorePathEdit->text());

    if (!directory.isEmpty()) {
        mStorePathEdit->setText(directory);
    }
}

void NewProjectDialog::accept()
{
    QString location = mStorePathEdit->text();
    QDir dir = QDir(location);
    if (!dir.exists()){
        dir.mkdir(location);
        qDebug() << "create Projects location: " << location;
    }
    QDialog::accept();
}

void NewProjectDialog::onNameChanged(const QString & text)
{
    QString storePath = getSVProjectsLocation() + "/" + text;
    mStorePathEdit->setText(storePath);
}

QPushButton *NewProjectDialog::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

AddFilesDialog::AddFilesDialog(const QString &storePath, QWidget *parent)
    : QDialog(parent),
    mCurrentPath(storePath)
{
    QStringList dirTreeHeaderList;
    dirTreeHeaderList << "Directory";
    mDirTreeModel = new FileSystemModel(dirTreeHeaderList);
    mDirTreeModel->setRootPath("");
    mDirTreeModel->setFilter(QDir::Dirs | QDir::NoDotAndDotDot);
    mDirTreeView = new QTreeView();
    mDirTreeView->setModel(mDirTreeModel);
    mDirTreeView->hideColumn(1);
    mDirTreeView->hideColumn(2);
    mDirTreeView->hideColumn(3);
    QModelIndex curIndex = mDirTreeModel->index(mCurrentPath);
    mDirTreeView->scrollTo(curIndex);
    mDirTreeView->setCurrentIndex(curIndex);
    connect(mDirTreeView->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(dirSelected(const QModelIndex &, const QModelIndex &)));

    mCurDirTableWidget = new QTableWidget(0, 1);
    mCurDirTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    mCurDirTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mCurDirTableWidget->horizontalHeader()->setHighlightSections(false);

    QStringList labels;
    labels << tr("File Name");
    mCurDirTableWidget->setHorizontalHeaderLabels(labels);
    mCurDirTableWidget->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft);

    mCurDirTableWidget->verticalHeader()->hide();
    mCurDirTableWidget->setShowGrid(false);
    mCurDirTableWidget->horizontalHeader()->setStretchLastSection(true);
    mCurDirTableWidget->verticalHeader()->setDefaultSectionSize(20);
    connect(mCurDirTableWidget, SIGNAL(cellActivated(int,int)),
            this, SLOT(cdDirOrAddFileToProject(int,int)));

    mCurPathLabel = new QLabel(mCurrentPath);
    mFileListTitle = new QLabel(tr("Project Files : (0)"));

    mFileListView = new StandardItemListView;
    mFileListModel = new QStandardItemModel;
    mFileListView->setModel(mFileListModel);
    mFileListView->setSelectionMode(QAbstractItemView::NoSelection);
    mFileListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    mAddButton = createButton(tr("Add"), SLOT(accept()));
    mAddAllButton = createButton(tr("AddAll"), SLOT(accept()));
    mAddTreeButton = createButton(tr("AddTree"), SLOT(accept()));
    mRemoveButton = createButton(tr("Remove"), SLOT(accept()));
    mRemoveAllButton = createButton(tr("RemoveAll"), SLOT(accept()));
    mRemoveTreeButton = createButton(tr("RemoveTree"), SLOT(accept()));

    mOKButton = createButton(tr("OK"), SLOT(accept()));
    mCancelButton = createButton(tr("Cancel"), SLOT(reject()));

    QVBoxLayout *addFilesButtonLayout = new QVBoxLayout();
    addFilesButtonLayout->addWidget(mOKButton);
    addFilesButtonLayout->addWidget(mAddButton);
    addFilesButtonLayout->addWidget(mAddAllButton);
    addFilesButtonLayout->addWidget(mAddTreeButton);

    QHBoxLayout *addFilesLayout = new QHBoxLayout();
    addFilesLayout->addWidget(mDirTreeView);
    addFilesLayout->addWidget(mCurDirTableWidget);
    addFilesLayout->addLayout(addFilesButtonLayout);

    QVBoxLayout *removeFilesButtonLayout = new QVBoxLayout();
    removeFilesButtonLayout->addWidget(mRemoveButton);
    removeFilesButtonLayout->addWidget(mRemoveAllButton);
    removeFilesButtonLayout->addWidget(mRemoveTreeButton);
    removeFilesButtonLayout->addWidget(mCancelButton);

    QHBoxLayout *removeFilesLayout = new QHBoxLayout();
    removeFilesLayout->addWidget(mFileListView);
    removeFilesLayout->addLayout(removeFilesButtonLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addWidget(mCurPathLabel);
    mainLayout->addLayout(addFilesLayout);
    mainLayout->addWidget(mFileListTitle);
    mainLayout->insertSpacing(2, 20);
    mainLayout->addLayout(removeFilesLayout);
    setLayout(mainLayout);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setWindowTitle(tr("New Project"));

    showFolder();
}

void AddFilesDialog::showFolder()
{
    QDir dir = QDir(mCurrentPath);
    QStringList files = dir.entryList(QStringList(), QDir::AllEntries | QDir::NoDot);
    showFiles(files);
}

void AddFilesDialog::cdDirOrAddFileToProject(int row, int /* column */)
{
    QTableWidgetItem *item = mCurDirTableWidget->item(row, 0);
    QString itemText = item->text();

    QDir dir = QDir(mCurrentPath);
    QString childPath = dir.absoluteFilePath(itemText);
    if (QFileInfo(childPath).isDir()){
        if(itemText == ".."){
            dir.cdUp();
            mCurrentPath = dir.path();
        }
        else{
            mCurrentPath = childPath;
        }
        mCurPathLabel->setText(mCurrentPath);
        showFolder();
    }
    else{
        //TODO: add this file to project file list
        qDebug() << "This is a file";
    }
}

void AddFilesDialog::dirSelected(const QModelIndex & current, const QModelIndex & /* previous */)
{
    mCurrentPath = mDirTreeModel->filePath(current);
    showFolder();
}

void AddFilesDialog::showFiles(const QStringList &files)
{
    while (mCurDirTableWidget->rowCount() != 0)
        mCurDirTableWidget->removeRow(0);

    QDir dir = QDir(mCurrentPath);
    for (int i = 0; i < files.size(); ++i) {
        mCurDirTableWidget->insertRow(i);

        QFile file(dir.absoluteFilePath(files[i]));
        QFileInfo fileInfo = QFileInfo(file);

        // File Name
        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        QFileIconProvider fileIcon;
        fileNameItem->setIcon(fileIcon.icon(fileInfo));
        mCurDirTableWidget->setItem(i, 0, fileNameItem);

//        // File Size
//        qint64 size = fileInfo.size();
//        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB")
//                                             .arg(int((size + 1023) / 1024)));
//        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
//        sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);
//        mCurDirTableWidget->setItem(i, 1, sizeItem);
    }
}

QPushButton *AddFilesDialog::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}
