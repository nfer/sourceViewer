
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
    mCurPathEdit = new QLineEdit(storePath);
    connect(mCurPathEdit, SIGNAL(returnPressed()),
            this, SLOT(curPathInput()));

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
            this, SLOT(curDirCellActivated(int,int)));

    mFileListTitle = new QLabel(tr("Project Files : (0)"));

    mFileListView = new StandardItemListView;
    mFileListModel = new QStandardItemModel;
    mFileListView->setModel(mFileListModel);
    mFileListView->setSelectionMode(QAbstractItemView::NoSelection);
    mFileListView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    mAddButton = createButton(tr("Add"), SLOT(cdDirOrAddFile()));
    mAddButton->setDefault(true);
    mAddAllButton = createButton(tr("AddAll"), SLOT(addAll()));
    mAddTreeButton = createButton(tr("AddTree"), SLOT(accept()));
    mRemoveButton = createButton(tr("Remove"), SLOT(accept()));
    mRemoveAllButton = createButton(tr("RemoveAll"), SLOT(accept()));
    mRemoveTreeButton = createButton(tr("RemoveTree"), SLOT(accept()));

    mOKButton = createButton(tr("OK"), SLOT(accept()));
    mCancelButton = createButton(tr("Cancel"), SLOT(reject()));

    QHBoxLayout *curPathLayout = new QHBoxLayout();
    curPathLayout->addWidget(mCurPathEdit);
    curPathLayout->addWidget(mOKButton);

    QVBoxLayout *addFilesButtonLayout = new QVBoxLayout();
//    addFilesButtonLayout->addWidget(mOKButton);
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
//    removeFilesButtonLayout->addWidget(mCancelButton);

    QHBoxLayout *removeFilesLayout = new QHBoxLayout();
    removeFilesLayout->addWidget(mFileListView);
    removeFilesLayout->addLayout(removeFilesButtonLayout);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(curPathLayout);
    mainLayout->addLayout(addFilesLayout);
    mainLayout->addWidget(mFileListTitle);
    mainLayout->insertSpacing(2, 20);
    mainLayout->addLayout(removeFilesLayout);
    setLayout(mainLayout);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setWindowTitle(tr("New Project"));

    updateTreeView(storePath);
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

void AddFilesDialog::curDirCellActivated(int row, int /* column */)
{
    QTableWidgetItem *item = mCurDirTableWidget->item(row, 0);
    if (NULL != item){
        //qDebug() << "cdDirOrAddFileToProject " << row << ", item->text() " << item->text();
        cdDirOrAddFileToList(item->text());
    }
}

void AddFilesDialog::cdDirOrAddFileToList(QString itemText)
{
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

        updateTreeView(mCurrentPath);
    }
    else{
        mCurDirTableWidget->removeRow(mCurDirTableWidget->currentRow());
        QStandardItem *item = new QStandardItem(childPath);
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

void AddFilesDialog::curPathInput()
{
//    qDebug() << "curPathInput " << mCurPathEdit->text();
    updateTreeView(mCurPathEdit->text());
}

void AddFilesDialog::cdDirOrAddFile()
{
    int row = mCurDirTableWidget->currentRow();
//    qDebug() << "cdDirOrAddFile() " << row;
    if (-1 != row){
        curDirCellActivated(row, 0);
    }
}

void AddFilesDialog::addAll()
{
    AddAllDialog dialog(this);
    if (dialog.exec() == QDialog::Rejected){
        qDebug() << "AddAllDialog is Rejected.";
        return;
    }
//    qDebug() << "isRecursively() " << dialog.isRecursively();

    QStringList files;
    searchFiles(mCurrentPath, files, dialog.isRecursively());

    QDir dir(mCurrentPath);
    QStringList curFileList = dir.entryList(QDir::Files);

    for(int i=0; i<curFileList.count(); i++){
//        qDebug()<< "curFileList" << curFileList.at(i);
        QList<QTableWidgetItem *> list = mCurDirTableWidget->findItems(curFileList.at(i), Qt::MatchExactly);
        if (list.count() != 0){
//            qDebug()<< "list[0] " << list[0]->text();
            mCurDirTableWidget->setCurrentItem(list[0]);
            mCurDirTableWidget->removeRow(mCurDirTableWidget->currentRow());
        }
    }

    for(int i=0;i<files.count();i++)
    {
//        qDebug()<<files.at(i);
        // FIXME1: check duplicate in mFileListModel
        QStandardItem *item = new QStandardItem(files.at(i));
        mFileListModel->appendRow(item);
    }
    mFileListModel->sort(0);
}

void AddFilesDialog::searchFiles(QString path, QStringList& fileList, bool isRecursively)
{
//    qDebug() << "isRecursively() " << isRecursively;
    QDir dir(path);
    QStringList curFileList = dir.entryList(QDir::Files);

    QString fullPath;
    for ( int i=0; i<curFileList.count(); i++){
        fullPath = path + "/" + curFileList.at(i);
        curFileList.replace(i, fullPath);
    }
    fileList += curFileList;

    QStringList dirList;
    dirList = dir.entryList(QDir::Dirs);
    dirList.removeAt(1);    //remove ".."
    dirList.removeAt(0);    //remove "."

    if (isRecursively){
        // Recursively Add Children
        for ( int i=0; i<dirList.count(); i++){
            searchFiles(path + "/" + dirList.at(i), fileList, isRecursively);
        }
    }
}

void AddFilesDialog::showFiles(const QStringList &files)
{
    while (mCurDirTableWidget->rowCount() != 0)
        mCurDirTableWidget->removeRow(0);

    mCurPathEdit->setText(mCurrentPath);
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

//    mCurDirTableWidget->setCurrentCell(0, 0);
    mSortOrder = Qt::AscendingOrder;
}

QPushButton *AddFilesDialog::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

void AddFilesDialog::updateTreeView(const QString & path)
{
    QModelIndex curIndex = mDirTreeModel->index(path);
    if (curIndex.isValid()){
        mDirTreeView->scrollTo(curIndex);
        mDirTreeView->setCurrentIndex(curIndex);
        mDirTreeView->expand(curIndex);
    }
}

bool AddFilesDialog::eventFilter(QObject*obj,QEvent*event)
{
    // here we only handle KeyPress event
    if (event->type() != QEvent::KeyPress)
        return QObject::eventFilter(obj,event);

    QKeyEvent*keyEvent = static_cast<QKeyEvent*>(event);
    int key = keyEvent->key();
    bool ret = false;

    if(obj == mCurDirTableWidget)
    {
        switch(key)
        {
        case Qt::Key_Return:
        case Qt::Key_Enter:
            curDirCellActivated(mCurDirTableWidget->currentRow(), 0);
            ret = true;
            break;

        case Qt::Key_Backtab:
            this->focusNextPrevChild(false);
            ret = true;
            break;

        case Qt::Key_Tab:
            this->focusNextPrevChild(true);
            ret = true;
            break;

        default:
            break;
        }
    }
    else if(obj == mDirTreeView)
    {
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
            ret = true;
            break;

        default:
            break;
        }
    }
    else{
        //return QObject::eventFilter(obj,event);
    }

    if (ret)
        return true;
    else
        return QObject::eventFilter(obj,event);
}

AddAllDialog::AddAllDialog(QWidget *parent)
    : QDialog(parent)
{
    mRecursively = new QCheckBox("&Recursively add sub-directories", this);
    mRecursively->setFixedWidth(240);

    mOKButton = createButton(tr("&OK"), SLOT(accept()));
    mCancelButton = createButton(tr("&Cancel"), SLOT(reject()));

    QVBoxLayout *leftLayout = new QVBoxLayout();
    leftLayout->setAlignment(Qt::AlignTop);
    leftLayout->addWidget(mRecursively);

    QVBoxLayout *rightLayout = new QVBoxLayout();
    rightLayout->addWidget(mOKButton);
    rightLayout->addWidget(mCancelButton);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(leftLayout);
    mainLayout->addLayout(rightLayout);
    setLayout(mainLayout);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);
    setWindowModality(Qt::WindowModal);

    setWindowTitle(tr("Add To Project"));
}

QPushButton *AddAllDialog::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}
