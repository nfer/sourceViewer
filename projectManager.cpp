
#include "stable.h"

#include "projectManager.h"
#include "config.h"

NewProjectDialog::NewProjectDialog(QWidget *parent)
    : QDialog(parent)
{
    // project name label and edit
    mNameLabel = new QLabel(tr("New Project Name..."));
    QString defaultName = "Unitled Project";
    mNameEdit = new QLineEdit(defaultName);
    mNameEdit->setMinimumWidth(360);
    connect(mNameEdit, SIGNAL(textChanged(const QString & )),
                this, SLOT(onNameChanged(const QString & )));

    // project store path label, edit and browser button
    mProjStorePathLabel = new QLabel(tr("Where do you want to store the project data file?"));
    QString defaultPath = getSVProjectsLocation() + "/" + defaultName;
    mProjStorePathEdit = new QLineEdit(defaultPath);
    mProjStorePathEdit->setMinimumWidth(360);
    mProjStorePathButton = createButton(tr("&Browse..."), SLOT(browseProjStorePath()));

    // source root path label, edit and browser button
    mSrcRootPathLabel = new QLabel(tr("Where is the source root path?"));
    mSrcRootPathEdit = new QLineEdit();
    mSrcRootPathEdit->setMinimumWidth(360);
    mSrcRootPathButton = createButton(tr("&Browse..."), SLOT(browseSrcRootPath()));

    mNextButton = createButton(tr("&Next"), SLOT(accept()));
    mCancelButton = createButton(tr("&Cancel"), SLOT(reject()));
    mNextButton->setDefault(true);

    QVBoxLayout * nameLayout = new QVBoxLayout();
    nameLayout->addWidget(mNameLabel);
    nameLayout->addWidget(mNameEdit);

    QGridLayout * projStorePath = new QGridLayout();
    projStorePath->addWidget(mProjStorePathLabel);
    projStorePath->addWidget(mProjStorePathEdit, 1, 0);
    projStorePath->addWidget(mProjStorePathButton, 1, 1);

    QGridLayout * srcRootPath = new QGridLayout();
    srcRootPath->addWidget(mSrcRootPathLabel);
    srcRootPath->addWidget(mSrcRootPathEdit, 1, 0);
    srcRootPath->addWidget(mSrcRootPathButton, 1, 1);

    QBoxLayout * naviLayout = new QBoxLayout(QBoxLayout::LeftToRight);
    naviLayout->addWidget(mCancelButton, 0, Qt::AlignLeft);
    naviLayout->addWidget(mNextButton, 0, Qt::AlignRight);

    QVBoxLayout *mainLayout = new QVBoxLayout();
    mainLayout->addLayout(nameLayout);
    mainLayout->insertSpacing(1, 20);
    mainLayout->addLayout(projStorePath);
    mainLayout->addLayout(srcRootPath);
    mainLayout->insertSpacing(4, 20);
    mainLayout->addLayout(naviLayout);
    setLayout(mainLayout);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setWindowTitle(tr("New Project"));
}

void NewProjectDialog::browseProjStorePath()
{
    QString directory = QFileDialog::getExistingDirectory(this,
                                   tr("Browse Files"),
                                   mProjStorePathEdit->text());

    if (!directory.isEmpty()) {
        mProjStorePathEdit->setText(directory);
    }
}

void NewProjectDialog::browseSrcRootPath()
{
    QString directory = QFileDialog::getExistingDirectory(this,
                                   tr("Browse Files"),
                                   "/");

    if (!directory.isEmpty()) {
        mSrcRootPathEdit->setText(directory);
    }
}

void NewProjectDialog::accept()
{
    bool isExist = Utils::enstance()->isProjectExist(mNameEdit->text());
    if (isExist){
        QMessageBox::warning(this, tr("Application"),
                             tr("Already has project with the same name.\nPlease input another one."),
                             QMessageBox::Ok);
        return;
    }

    QString srcRootPath = mSrcRootPathEdit->text();
    if (srcRootPath == ""){
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Application"),
                     tr("You haven't set source root path.\nDo you continue?"),
                     QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::No){
            return;
        }
    }

    QString location = mProjStorePathEdit->text();
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
    mProjStorePathEdit->setText(storePath);
}

QPushButton *NewProjectDialog::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

AddFilesDialog::AddFilesDialog(const QString & projName, const QString & projStorePath,
        const QString & srcRootPath, QWidget *parent)
    : QDialog(parent),
    mProjName(projName),
    mProjStorePath(projStorePath),
    mSrcRootPath(srcRootPath),
    mCurrentPath(srcRootPath)
{
    mUtils = Utils::enstance();
    mUtils->setCurrentProject(mProjName, mProjStorePath);

    mUtils->readStringList(IGNOREFOLDERLIST, mIgnoreFolderList);
    mUtils->readStringList(IGNOREFILELIST, mIgnoreFileList);
    mIgnoreSuffixList = mIgnoreFileList.filter(QRegExp("^\\*\\..*")); // suffix like *.txt

    mCurPathEdit = new QLineEdit(srcRootPath);

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
    mDirTreeView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mDirTreeView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(mDirTreeView->selectionModel(), SIGNAL(currentChanged(const QModelIndex &, const QModelIndex &)),
            this, SLOT(dirSelected(const QModelIndex &, const QModelIndex &)));

    mCurDirTableWidget = new QTableWidget(0, 1);
    mCurDirTableWidget->setSelectionBehavior(QAbstractItemView::SelectRows);
    mCurDirTableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mCurDirTableWidget->setShowGrid(false); // do not show grid line
    mCurDirTableWidget->installEventFilter(this);

    mCurDirTableWidget->setHorizontalHeaderLabels(QStringList("File Name"));
    mCurDirTableWidget->horizontalHeaderItem(0)->setTextAlignment(Qt::AlignLeft);
    mCurDirTableWidget->horizontalHeader()->setHighlightSections(false);
    mCurDirTableWidget->horizontalHeader()->setStretchLastSection(true);
    connect(mCurDirTableWidget->horizontalHeader(), SIGNAL(sectionClicked(int )),
            this, SLOT(sortCurDir(int)));
    mCurDirTableWidget->verticalHeader()->hide();
    mCurDirTableWidget->verticalHeader()->setDefaultSectionSize(20);
    connect(mCurDirTableWidget, SIGNAL(cellDoubleClicked(int,int)),
            this, SLOT(curDirDoubleClicked(int,int)));
    connect(mCurDirTableWidget, SIGNAL(currentItemChanged(QTableWidgetItem * , QTableWidgetItem * )),
            this, SLOT(curDirItemChanged(QTableWidgetItem * , QTableWidgetItem * )));

    mFileListTitle = new QLabel(tr("Project Files : (0)"));

    mFileListView = new QListView;
    mFileListModel = new QStandardItemModel;
    mFileListView->setModel(mFileListModel);
    mFileListView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mFileListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mFileListView->setAlternatingRowColors(true);
    connect(mFileListView, SIGNAL(doubleClicked(const QModelIndex &)),
            this, SLOT(fileListDoubleClicked(const QModelIndex &)));

    mAddFileButton = createButton(tr("Add"), SLOT(addFile()));
    mAddAllButton = createButton(tr("AddAll"), SLOT(addAll()));
    mAddTreeButton = createButton(tr("AddTree"), SLOT(addTree()));
    mRemoveFileButton = createButton(tr("Remove"), SLOT(removeFile()));
    mRemoveAllButton = createButton(tr("RemoveAll"), SLOT(removeAll()));
    mRemoveTreeButton = createButton(tr("RemoveTree"), SLOT(removeTree()));
    mAddFileButton->setDefault(true);

    // set default disable state
    mAddFileButton->setDisabled(true);
    mAddTreeButton->setDisabled(true);
    mRemoveFileButton->setDisabled(true);
    mRemoveAllButton->setDisabled(true);
    mRemoveTreeButton->setDisabled(true);

    mOKButton = createButton(tr("OK"), SLOT(accept()));

    QHBoxLayout *curPathLayout = new QHBoxLayout();
    curPathLayout->addWidget(mCurPathEdit);
    curPathLayout->addWidget(mOKButton);

    QVBoxLayout *addFilesButtonLayout = new QVBoxLayout();
    addFilesButtonLayout->addWidget(mAddFileButton);
    addFilesButtonLayout->addWidget(mAddAllButton);
    addFilesButtonLayout->addWidget(mAddTreeButton);

    QHBoxLayout *addFilesLayout = new QHBoxLayout();
    addFilesLayout->addWidget(mDirTreeView);
    addFilesLayout->addWidget(mCurDirTableWidget);
    addFilesLayout->addLayout(addFilesButtonLayout);

    QVBoxLayout *removeFilesButtonLayout = new QVBoxLayout();
    removeFilesButtonLayout->addWidget(mRemoveFileButton);
    removeFilesButtonLayout->addWidget(mRemoveAllButton);
    removeFilesButtonLayout->addWidget(mRemoveTreeButton);

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

    updateTreeView(srcRootPath);
}

void AddFilesDialog::accept()
{
    QString fileName = mProjStorePath + "/" + mProjName + ".filelist";
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

    mUtils->writeString(SRCROOTPATH, mSrcRootPath);
    mUtils->addProject(mProjName, mProjStorePath);

    QDialog::accept();
}

void AddFilesDialog::showFolder()
{
    QDir dir = QDir(mCurrentPath);
    QStringList files = dir.entryList(QStringList(), QDir::AllEntries | QDir::NoDot);
    showFiles(files);
}

void AddFilesDialog::curDirDoubleClicked(int row, int /* column */)
{
    QTableWidgetItem *item = mCurDirTableWidget->item(row, 0);
    if (NULL != item){
        cdDirOrAddFileToList(item->text());
    }
}

void AddFilesDialog::fileListDoubleClicked(const QModelIndex & index)
{
    if (!index.isValid()){
        qDebug() << "fileListDoubleClicked, but index isn't valid.";
        return;
    }

    QStandardItem * item = mFileListModel->itemFromIndex(index);
    if (NULL == item || item->text().isEmpty()){
        qDebug() << "fileListDoubleClicked, but item is NULL.";
        return;
    }

    QFileInfo fileInfo(item->text());
    if (fileInfo.path() == mCurrentPath){
        // add this file to mCurDirTableWidget
        int rowCount = mCurDirTableWidget->rowCount();

        QTableWidgetItem *fileNameItem = new QTableWidgetItem(fileInfo.fileName());
        QFileIconProvider fileIcon;
        fileNameItem->setIcon(fileIcon.icon(fileInfo));

        mCurDirTableWidget->insertRow(rowCount);
        mCurDirTableWidget->setItem(rowCount, 0, fileNameItem);

        mCurDirTableWidget->sortItems(0, mSortOrder);
    }

    mFileListModel->removeRow(index.row());
    if (mFileListModel->rowCount() == 0){
        mRemoveFileButton->setDisabled(true);
        mRemoveAllButton->setDisabled(true);
    }

    updateFileListTitle();
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

        updateFileListTitle();

        mRemoveFileButton->setDisabled(false);
        mRemoveAllButton->setDisabled(false);
    }
}

void AddFilesDialog::dirSelected(const QModelIndex & current, const QModelIndex & /* previous */)
{
    mCurrentPath = mDirTreeModel->filePath(current);
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

void AddFilesDialog::addFile()
{
    int row = mCurDirTableWidget->currentRow();
    if (-1 != row){
        curDirDoubleClicked(row, 0);
    }
}

void AddFilesDialog::addAll()
{
    AddAllDialog dialog(this);
    if (dialog.exec() == QDialog::Rejected){
        qDebug() << "AddAllDialog is Rejected.";
        return;
    }

    // add all files(isRecursively) to mFileListModel
    QStringList files;
    searchFiles(mCurrentPath, files, dialog.isRecursively());
    for(int i=0;i<files.count();i++)
    {
        // if file not in
        QList<QStandardItem *> list = mFileListModel->findItems(files.at(i), Qt::MatchExactly);
        if (list.count() == 0){
            QStandardItem *item = new QStandardItem(files.at(i));
            mFileListModel->appendRow(item);
        }
    }

    if (mFileListModel->rowCount() != 0){
        mFileListModel->sort(0);
        mRemoveFileButton->setDisabled(false);
        mRemoveAllButton->setDisabled(false);

        updateFileListTitle();
    }

    // remove all files in mCurDirTableWidget
    QDir dir(mCurrentPath);
    QStringList curFileList = dir.entryList(QDir::Files);
    for(int i=0; i<curFileList.count(); i++){
        QList<QTableWidgetItem *> list = mCurDirTableWidget->findItems(curFileList.at(i), Qt::MatchExactly);
        if (list.count() != 0){
            mCurDirTableWidget->setCurrentItem(list[0]);
            mCurDirTableWidget->removeRow(mCurDirTableWidget->currentRow());
        }
    }
}

void AddFilesDialog::addTree()
{
    QTableWidgetItem * item = mCurDirTableWidget->currentItem();
    if (NULL == item){
        return;
    }
    QString folder = item->text();

    // add all files(isRecursively) to mFileListModel
    QStringList files;
    searchFiles(mCurrentPath + "/" + folder, files, true);
    for(int i=0;i<files.count();i++)
    {
        // if file not in
        QList<QStandardItem *> list = mFileListModel->findItems(files.at(i), Qt::MatchExactly);
        if (list.count() == 0){
            QStandardItem *item = new QStandardItem(files.at(i));
            mFileListModel->appendRow(item);
        }
    }

    if (mFileListModel->rowCount() != 0){
        mFileListModel->sort(0);
        mRemoveFileButton->setDisabled(false);
        mRemoveAllButton->setDisabled(false);

        updateFileListTitle();
    }
}

void AddFilesDialog::removeFile()
{
    QModelIndex index = mFileListView->currentIndex();
    if (index.isValid()){
        fileListDoubleClicked(index);
    }
}

void AddFilesDialog::removeAll()
{
    mFileListModel->clear();

    showFolder();

    if (mFileListModel->rowCount() == 0){
        mRemoveFileButton->setDisabled(true);
        mRemoveAllButton->setDisabled(true);
    }

    updateFileListTitle();
}

void AddFilesDialog::removeTree()
{
    QTableWidgetItem * item = mCurDirTableWidget->currentItem();
    if (NULL == item){
        return;
    }
    QString folder = item->text();

    // add all files(isRecursively) to mFileListModel
    QStringList files;
    searchFiles(mCurrentPath + "/" + folder, files, true);
    for(int i=0;i<files.count();i++)
    {
        // if file in mFileListView, remove it
        QList<QStandardItem *> list = mFileListModel->findItems(files.at(i), Qt::MatchExactly);
        if (list.count() != 0){
            QModelIndex index = mFileListModel->indexFromItem(list[0]);
            mFileListModel->removeRow(index.row());
        }
    }

    if (mFileListModel->rowCount() == 0){
        mRemoveFileButton->setDisabled(true);
        mRemoveAllButton->setDisabled(true);
    }

    updateFileListTitle();
}

void AddFilesDialog::curDirItemChanged(QTableWidgetItem * current, QTableWidgetItem * /* previous */)
{
    if (current == NULL || current->text() == ".."){
        mAddFileButton->setDisabled(true);
        mAddTreeButton->setDisabled(true);
        mRemoveTreeButton->setDisabled(true);
        return ;
    }

    QFileInfo fileInfo(mCurrentPath + "/" + current->text());
    if (fileInfo.isDir()){
        mAddTreeButton->setDisabled(false);
        mRemoveTreeButton->setDisabled(false);
    }
    else{
        mAddTreeButton->setDisabled(true);
        mRemoveTreeButton->setDisabled(true);
    }

    mAddFileButton->setDisabled(false);
}

void AddFilesDialog::searchFiles(QString path, QStringList& fileList, bool isRecursively)
{
    QDir dir(path);
    QStringList curFileList = dir.entryList(QDir::Files);

    QString fileName;
    QString fullPath;
    for ( int i=0; i<curFileList.count(); i++){
        fileName = curFileList.at(i);
        fullPath = path + "/" + fileName;

        if (mUtils->isIgnoredFile(fileName, mIgnoreFileList) ||
            mUtils->isIgnoredFile(fullPath, mIgnoreFileList) ||
            mUtils->IsIgnoreSuffix(fileName, mIgnoreSuffixList)){
            continue;
        }

        fileList += fullPath;
    }

    if (isRecursively){
        QStringList dirList;
        dirList = dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

        // Recursively Add Children
        for ( int i=0; i<dirList.count(); i++){
            fileName = curFileList.at(i);
            fullPath = path + "/" + fileName;

            if (mUtils->isIgnoredFolder(fileName, mIgnoreFolderList) ||
                mUtils->isIgnoredFolder(fullPath, mIgnoreFolderList)){
                continue;
            }

            searchFiles(fullPath, fileList, isRecursively);
        }
    }
}

void AddFilesDialog::updateFileListTitle()
{
    mFileListTitle->setText(tr("Project Files : (%1)").arg(mFileListModel->rowCount()));
}

void AddFilesDialog::showFiles(const QStringList &files)
{
    while (mCurDirTableWidget->rowCount() != 0)
        mCurDirTableWidget->removeRow(0);

    mCurPathEdit->setText(mCurrentPath);
    QDir dir = QDir(mCurrentPath);

    int rowCount = 0;
    QString fileName;
    QString fullPath;
    for (int i = 0; i < files.size(); ++i) {
        fileName = files[i];
        fullPath = dir.absoluteFilePath(fileName);

        if (mFileListModel->rowCount() != 0){
            QList<QStandardItem *> list = mFileListModel->findItems(fullPath);
            if (list.size() != 0){
                continue;
            }
        }

        QFile file(fullPath);
        QFileInfo fileInfo = QFileInfo(file);

        if (fileInfo.isDir() && mIgnoreFolderList.size() != 0){
            if (mUtils->isIgnoredFolder(fileInfo.fileName(), mIgnoreFolderList) ||
                mUtils->isIgnoredFolder(fileInfo.absoluteFilePath(), mIgnoreFolderList))
                continue;
        }

        if (fileInfo.isFile() && mIgnoreFileList.size() != 0){
            if (mUtils->isIgnoredFile(fileInfo.fileName(), mIgnoreFileList) ||
                mUtils->isIgnoredFile(fileInfo.absoluteFilePath(), mIgnoreFileList) ||
                mUtils->IsIgnoreSuffix(fileInfo.fileName(), mIgnoreSuffixList))
                continue;
        }

        // File Name
        QTableWidgetItem *fileNameItem = new QTableWidgetItem(fileName);
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
            curDirDoubleClicked(mCurDirTableWidget->currentRow(), 0);
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

OpenProjectDialog::OpenProjectDialog(QWidget *parent)
    : QDialog(parent)
{
    mUtils = Utils::enstance();

    // project name label and edit
    mNameLabel = new QLabel(tr("Project Name:"));
    mNameEdit = new QLineEdit();
    mNameEdit->setMinimumWidth(240);
    connect(mNameEdit, SIGNAL(textChanged(const QString & )),
                this, SLOT(onNameChanged(const QString & )));


    mProjListView = new QListView;
    mProjListModel = new QStandardItemModel;
    mProjListView->setModel(mProjListModel);
    mProjListView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mProjListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mProjListView->setAlternatingRowColors(true);
    connect(mProjListView, SIGNAL(doubleClicked(const QModelIndex &)),
            this, SLOT(projListDoubleClicked(const QModelIndex &)));

    QStringList projNameList = mUtils->getProjNameList();
    QString projName;
    for (int i = 0; i < projNameList.size(); i++){
        projName = projNameList.at(i);
        QStandardItem *item = new QStandardItem(projName);
        mProjListModel->appendRow(item);
    }
    if (mProjListModel->rowCount() > 1)
        mProjListModel->sort(0);

    mBrowseButton = createButton(tr("&Browse..."), SLOT(browse()));
    mOKButton = createButton(tr("&OK"), SLOT(accept()));
    mCancelButton = createButton(tr("&Cancel"), SLOT(reject()));

    QVBoxLayout * projLayout = new QVBoxLayout();
    projLayout->addWidget(mNameLabel);
    projLayout->addWidget(mNameEdit);
    projLayout->addWidget(mProjListView);

    QBoxLayout * naviLayout = new QBoxLayout(QBoxLayout::TopToBottom);
    naviLayout->addWidget(mOKButton);
    naviLayout->addWidget(mCancelButton);
    naviLayout->addWidget(mBrowseButton, 0, Qt::AlignBottom);

    QHBoxLayout *mainLayout = new QHBoxLayout();
    mainLayout->addLayout(projLayout);
    mainLayout->addLayout(naviLayout);
    setLayout(mainLayout);

    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    setWindowTitle(tr("Open Project"));
}

void OpenProjectDialog::accept()
{
    QModelIndex index = mProjListView->currentIndex();
    if (index.isValid()){
        selectProjectByIndex(index);
    }

    QDialog::accept();
}

void OpenProjectDialog::browse()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Project"),
                                                getSVProjectsLocation(),
                                                tr("Project Files (*.project)"));
    if (!fileName.isEmpty())
        mNameEdit->setText(fileName);
}

void OpenProjectDialog::onNameChanged(const QString & text)
{
    qDebug() << "onNameChanged " << text;
}

void OpenProjectDialog::projListDoubleClicked(const QModelIndex & index )
{
    selectProjectByIndex(index);
    QDialog::accept();
}

QPushButton *OpenProjectDialog::createButton(const QString &text, const char *member)
{
    QPushButton *button = new QPushButton(text);
    connect(button, SIGNAL(clicked()), this, member);
    return button;
}

void OpenProjectDialog::selectProjectByIndex(const QModelIndex &index)
{
    mProjName = mProjListModel->itemFromIndex(index)->text();
    mProjStorePath = mUtils->getProjStorePath(mProjName);
    mUtils->setCurrentProject(mProjName, mProjStorePath);
}
