
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

AddFilesDialog::AddFilesDialog(const QString &projectName, const QString &storePath, QWidget *parent)
    : QDialog(parent),
    mProjectName(projectName),
    mStorePath(storePath),
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
    mDirTreeView->installEventFilter(this);
    connect(mDirTreeView->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(dirSelected(const QModelIndex &, const QModelIndex &)));

    mCurDirTableWidget = new QTableWidget(0, 1);
    mCurDirTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    mCurDirTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mCurDirTableWidget->horizontalHeader()->setHighlightSections(false);
    mCurDirTableWidget->installEventFilter(this);
    connect(mCurDirTableWidget->horizontalHeader(), SIGNAL(sectionClicked(int )),
            this, SLOT(sortCurDir(int)));

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

    QModelIndex curIndex = mDirTreeModel->index(mCurrentPath);
    mDirTreeView->setCurrentIndex(curIndex);
    mDirTreeView->scrollTo(curIndex);
}

void AddFilesDialog::accept()
{
    QString fileName = mStorePath + "/" + mProjectName + ".filelist";
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr(SV_PROGRAM_NAME),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        qWarning() << tr("Cannot write file %1:\n%2.").arg(fileName).arg(file.errorString());
        return ;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    QStandardItem * item;
    for (int i=0; i < mFileListModel->rowCount(); i++){
        item = mFileListModel->item(i);
//        qDebug() << "i " << i << ", item: " << item->text();
        out << item->text() << '\n';
    }

#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    QDialog::accept();
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
    //qDebug() << "cdDirOrAddFileToProject " << row << ", itemText " << itemText;

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
        QModelIndex curIndex = mDirTreeModel->index(mCurrentPath);
        mDirTreeView->scrollTo(curIndex);
        mDirTreeView->setCurrentIndex(curIndex);
    }
    else{
        QStandardItem *item = new QStandardItem(childPath);
        mCurDirTableWidget->removeRow(row);
        mFileListModel->appendRow(item);
        mFileListModel->sort(0);
    }
}

void AddFilesDialog::dirSelected(const QModelIndex & current, const QModelIndex & /* previous */)
{
    mCurrentPath = mDirTreeModel->filePath(current);
    //qDebug() << "dirSelectede " << current << ", mCurrentPath " << mCurrentPath;
    showFolder();
}

void AddFilesDialog::sortCurDir(int index)
{
    //qDebug() << "sortCurDir " << index << ", current sort order " << mSortOrder;

    if (mSortOrder == Qt::AscendingOrder) mSortOrder = Qt::DescendingOrder;
    else mSortOrder = Qt::AscendingOrder;
    mCurDirTableWidget->sortItems(index, mSortOrder);
}

void AddFilesDialog::showFiles(const QStringList &files)
{
    while (mCurDirTableWidget->rowCount() != 0)
        mCurDirTableWidget->removeRow(0);

    mCurPathLabel->setText(mCurrentPath);
    QDir dir = QDir(mCurrentPath);

    int rowCount = 0;
    for (int i = 0; i < files.size(); ++i) {
        QString fullPath = dir.absoluteFilePath(files[i]);

        if (mFileListModel->rowCount() != 0){
            QList<QStandardItem *> list = mFileListModel->findItems(fullPath);
            if (list.size() != 0){
//                qDebug() << "already has this file in file list." << fullPath;
                continue;
            }
        }

        QFile file(fullPath);
        QFileInfo fileInfo = QFileInfo(file);

        // File Name
        QTableWidgetItem *fileNameItem = new QTableWidgetItem(files[i]);
        QFileIconProvider fileIcon;
        fileNameItem->setIcon(fileIcon.icon(fileInfo));

        mCurDirTableWidget->insertRow(rowCount);
        mCurDirTableWidget->setItem(rowCount, 0, fileNameItem);

//        // File Size
//        qint64 size = fileInfo.size();
//        QTableWidgetItem *sizeItem = new QTableWidgetItem(tr("%1 KB")
//                                             .arg(int((size + 1023) / 1024)));
//        sizeItem->setTextAlignment(Qt::AlignRight | Qt::AlignVCenter);
//        sizeItem->setFlags(sizeItem->flags() ^ Qt::ItemIsEditable);
//        mCurDirTableWidget->setItem(i, 1, sizeItem);

        rowCount++;
    }

    mCurDirTableWidget->setCurrentCell(0, 0);
    mSortOrder = Qt::AscendingOrder;
}

QPushButton *AddFilesDialog::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

bool AddFilesDialog::eventFilter(QObject*obj,QEvent*event)
{
    if(obj == mCurDirTableWidget && event->type() == QEvent::KeyPress)
    {
        QKeyEvent*keyEvent = static_cast<QKeyEvent*>(event);//将事件转化为键盘事件
        int key = keyEvent->key();
        switch(key)
        {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            cdDirOrAddFileToProject(mCurDirTableWidget->currentRow(), 0);
            return true;

        case Qt::Key_Backtab:
            this->focusNextPrevChild(false);
            return true;

        case Qt::Key_Tab:
            this->focusNextPrevChild(true);
            return true;

        default:
            return QObject::eventFilter(obj,event);
        }
    }
    else if(obj == mDirTreeView && event->type() == QEvent::KeyPress)
    {
        QKeyEvent*keyEvent = static_cast<QKeyEvent*>(event);//将事件转化为键盘事件
        int key = keyEvent->key();
        switch(key)
        {
        case Qt::Key_Return:
        case Qt::Key_Enter:
        {
            QModelIndex index = mDirTreeView->currentIndex();
            if (mDirTreeView->isExpanded(index))
                mDirTreeView->collapse(index);
            else
                mDirTreeView->expand(index);
        }
            return true;

        default:
            return QObject::eventFilter(obj,event);
        }
    }
    else{
        return QObject::eventFilter(obj,event);
    }
}
