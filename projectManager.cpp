
#include "projectManager.h"
#include "component/PushButton.h"

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
    QString defaultPath = Utils::getProjectPath() + "/" + defaultName;
    mProjStorePathEdit = new QLineEdit(defaultPath);
    mProjStorePathEdit->setMinimumWidth(360);
    mProjStorePathButton = new PushButton(tr("&Browse..."), this, SLOT(browseProjStorePath()));

    // source root path label, edit and browser button
    mSrcRootPathLabel = new QLabel(tr("Where is the source root path?"));
    mSrcRootPathEdit = new QLineEdit();
    mSrcRootPathEdit->setMinimumWidth(360);
    mSrcRootPathButton = new PushButton(tr("&Browse..."), this, SLOT(browseSrcRootPath()));

    mNextButton = new PushButton(tr("&Next"), this, SLOT(accept()));
    mCancelButton = new PushButton(tr("&Cancel"), this, SLOT(reject()));
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

    Utils * utils = Utils::enstance();

    QString name = mNameEdit->text();
    QString storePath = mProjStorePathEdit->text();
    utils->addProject(name, storePath);
    utils->setCurrentProject(name, storePath);
    utils->writeString(SRCROOTPATH, srcRootPath);

    QDialog::accept();
}

void NewProjectDialog::onNameChanged(const QString & text)
{
    QString storePath = Utils::getProjectPath() + "/" + text;
    mProjStorePathEdit->setText(storePath);
}

// FIXME: remove srcRootPath from param, as it can be get from project setting
AddFilesDialog::AddFilesDialog(const QString & projName, const QString & projStorePath,
        const QString & srcRootPath, QWidget *parent)
    : QDialog(parent),
    mProjName(projName),
    mProjStorePath(projStorePath),
    mSrcRootPath(srcRootPath),
    mCurrentPath(srcRootPath)
{
    mUtils = Utils::enstance();

    mIgnoreFolderList = mUtils->readStringList(IGNOREFOLDERLIST);
    mIgnoreFileList = mUtils->readStringList(IGNOREFILELIST);
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

    initProjFileList();

    mAddFileButton = new PushButton(tr("Add"), this, SLOT(addFile()));
    mAddAllButton = new PushButton(tr("AddAll"), this, SLOT(addAll()));
    mAddTreeButton = new PushButton(tr("AddTree"), this, SLOT(addTree()));
    mRemoveFileButton = new PushButton(tr("Remove"), this, SLOT(removeFile()));
    mRemoveAllButton = new PushButton(tr("RemoveAll"), this, SLOT(removeAll()));
    mRemoveTreeButton = new PushButton(tr("RemoveTree"), this, SLOT(removeTree()));
    mAddFileButton->setDefault(true);

    // set default disable state
    mAddFileButton->setDisabled(true);
    mAddTreeButton->setDisabled(true);
    if (mFileListModel->rowCount() == 0){
        mRemoveFileButton->setDisabled(true);
        mRemoveAllButton->setDisabled(true);
    }
    mRemoveTreeButton->setDisabled(true);

    mOKButton = new PushButton(tr("OK"), this, SLOT(accept()));

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

void AddFilesDialog::initProjFileList()
{
    QString fileListFile = mUtils->getProjFileListFile();
    if (fileListFile.isEmpty()){
        return ;
    }

    QFile file(fileListFile);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileListFile)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    while(!in.atEnd()){
        QString line=in.readLine();
        QStandardItem *item = new QStandardItem(line);
        mFileListModel->appendRow(item);
    }

    mFileListModel->sort(0);
}

void AddFilesDialog::accept()
{
    QString fileName = mProjStorePath + "/" + mProjName + FILELIST_SUFFIX;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QString warningStr = tr("Cannot write file %1:\n%2.").arg(fileName).arg(file.errorString());
        QMessageBox::warning(this, Utils::getAppName(), warningStr);
        qWarning() << warningStr;
        return ;
    }

    QTextStream out(&file);
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif

    QStandardItem * item;
    for (int i=0; i < mFileListModel->rowCount(); i++){
        item = mFileListModel->item(i);
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

    mOKButton = new PushButton(tr("&OK"), this, SLOT(accept()));
    mCancelButton = new PushButton(tr("&Cancel"), this, SLOT(reject()));

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

ProjectListDialog::ProjectListDialog(QWidget *parent)
    : QDialog(parent)
{
    mUtils = Utils::enstance();

    // project name label and edit
    mNameLabel = new QLabel(tr("Project Name:"));
    mNameEdit = new QLineEdit();
    mNameEdit->setMinimumWidth(240);
    connect(mNameEdit, SIGNAL(textChanged(const QString & )),
                this, SLOT(onNameChanged(const QString & )));
    mNameEdit->installEventFilter(this);

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

    mBrowseButton = new PushButton(tr("&Browse..."), this, SLOT(browse()));
    mOKButton = new PushButton(tr("&OK"), this, SLOT(accept()));
    mCancelButton = new PushButton(tr("&Cancel"), this, SLOT(reject()));

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

void ProjectListDialog::accept()
{
    QModelIndex index = mProjListView->currentIndex();
    QString projName = mNameEdit->text();

    if (index.isValid()){
        selectProjectByIndex(index);
    }
    else if (Utils::isFullFilePath(projName) && Utils::isProjectFile(projName)){
        // this project is selected by browse()
        QFileInfo info = QFileInfo(projName);
        QString name = info.baseName();
        QString path = info.path();
        mUtils->setCurrentProject(name, path);

        if (!mUtils->isProjectExist(name)){
            mUtils->addProject(name, path);
        }
    }
    else{
        QMessageBox::warning(this, tr("Application"),
                             tr("Please select an exist project."),
                             QMessageBox::Ok);
        return ;
    }

    QDialog::accept();
}

void ProjectListDialog::browse()
{
    QString filter = tr("Project Files (*%1)").arg(PROJECT_SUFFIX);
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Project"),
                                                Utils::getProjectPath(),
                                                filter);
    if (!fileName.isEmpty())
        mNameEdit->setText(fileName);
}

void ProjectListDialog::onNameChanged(const QString & text)
{
    // FIXME: optimize later
    QModelIndex index;
    mProjListView->setCurrentIndex(index);

    if (text.isEmpty()){
        showAllRow();
        return;
    }

    QString name(text);
    QRegExp regExp = Utils::trRegExp(name, Qt::CaseInsensitive, QRegExp::WildcardUnix);

    for (int i = 0; i < mProjListModel->rowCount(); i++){
        QString projName = mProjListModel->item(i)->text();
        if (projName.contains(regExp)){
            mProjListView->setRowHidden(i, false);
            if (!index.isValid()){
                index = mProjListModel->index(i, 0);
                mProjListView->setCurrentIndex(index);
            }
        }
        else{
            mProjListView->setRowHidden(i, true);
        }
    }
}

void ProjectListDialog::showAllRow()
{
    for (int i = 0; i < mProjListModel->rowCount(); i++){
        mProjListView->setRowHidden(i, false);
    }
}

void ProjectListDialog::projListDoubleClicked(const QModelIndex & index )
{
    selectProjectByIndex(index);
    QDialog::accept();
}

void ProjectListDialog::selectProjectByIndex(const QModelIndex &index)
{
    mProjName = mProjListModel->itemFromIndex(index)->text();
    mProjStorePath = mUtils->getProjStorePath(mProjName);
    qDebug() << "mProjName is " << mProjName;
    qDebug() << "mProjStorePath is " << mProjStorePath;
}

bool ProjectListDialog::eventFilter(QObject*obj,QEvent*event)
{
    // here we only handle KeyPress event
    if (event->type() != QEvent::KeyPress)
        return QObject::eventFilter(obj,event);

    QKeyEvent*keyEvent = static_cast<QKeyEvent*>(event);
    int key = keyEvent->key();
    bool ret = false;

    if(obj == mNameEdit)
    {
        switch(key)
        {
        case Qt::Key_Up:
            selectDisplayItemByOffset(-1);
            ret = true;
            break;

        case Qt::Key_Down:
            selectDisplayItemByOffset(1);
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

void ProjectListDialog::selectDisplayItemByOffset(int offset)
{
    int start = -1, end = -1;
    int currentOffset = 0, idxOffset = 0;
    start = mProjListView->currentIndex().row();
    if(offset > 0){
        start++;
        end = mProjListModel->rowCount();
        idxOffset = 1;
    }
    else if(offset < 0){
        start--;
        end = -1;
        idxOffset = -1;
    }
    else{
        return ;
    }

    if (start >= mProjListModel->rowCount() || start < 0){
        return ;
    }

    for (int i = start; i != end; i += idxOffset){
        if (!mProjListView->isRowHidden(i)){
            currentOffset += idxOffset;
            if (currentOffset == offset){
                QModelIndex index = mProjListModel->index(i, 0);
                mProjListView->setCurrentIndex(index);
                return;
            }
        }
    }
}

OpenProjectDialog::OpenProjectDialog(QWidget *parent)
    : ProjectListDialog(parent)
{
    mUtils = Utils::enstance();

    setWindowTitle(tr("Open Project"));
}

void OpenProjectDialog::selectProjectByIndex(const QModelIndex &index)
{
    mProjName = mProjListModel->itemFromIndex(index)->text();
    mProjStorePath = mUtils->getProjStorePath(mProjName);
    mUtils->setCurrentProject(mProjName, mProjStorePath);
}

RemoveProjectDialog::RemoveProjectDialog(QWidget *parent)
    : ProjectListDialog(parent)
{
    mUtils = Utils::enstance();

    setWindowTitle(tr("Remove Project"));
}

void RemoveProjectDialog::selectProjectByIndex(const QModelIndex &index)
{
    mProjName = mProjListModel->itemFromIndex(index)->text();
    mUtils->removeProject(mProjName);
}
