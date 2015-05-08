
#include "config.h"
#include "mainwindow.h"
#include "projectManager.h"

MainWindow::MainWindow() :
    mProjName(),
    mProjStorePath()
{
    mUtils = Utils::enstance();

    textEdit = new QPlainTextEdit;
    setCentralWidget(textEdit);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    setupDockWidgets();

    readSettings();
    loadLayout(tr("mainwindow.layout"));

    connect(textEdit->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));

    setCurrentFile("");
    setUnifiedTitleAndToolBarOnMac(true);
    curEncodingAct = NULL;
    mCodec = NULL;
    mHasBOM = false;
    curEOLAct = NULL;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        saveLayout(tr("mainwindow.layout"));
        event->accept();
    } else {
        event->ignore();
    }
}

void MainWindow::saveLayout(QString fileName)
{
//    qDebug() << tr("saveLayout with fileName") << fileName;
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly)) {
        QString msg = tr("Failed to open %1\n%2")
                        .arg(fileName)
                        .arg(file.errorString());
        qWarning() << tr("Error") << msg;
        return;
    }

    QByteArray geo_data = saveGeometry();
    QByteArray layout_data = saveState();

    bool ok = file.putChar((uchar)geo_data.size());
    if (ok)
        ok = file.write(geo_data) == geo_data.size();
    if (ok)
        ok = file.write(layout_data) == layout_data.size();

    if (!ok) {
        QString msg = tr("Error writing to %1\n%2")
                        .arg(fileName)
                        .arg(file.errorString());
        qWarning() << tr("Error") << msg;
        return;
    }
}

void MainWindow::loadLayout(QString fileName)
{
//    qDebug() << tr("loadLayout with fileName") << fileName;
    if (fileName.isEmpty())
        return;
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly)) {
        QString msg = tr("Failed to open %1\n%2")
                        .arg(fileName)
                        .arg(file.errorString());
        qWarning() << tr("Error") << msg;
        return;
    }

    uchar geo_size;
    QByteArray geo_data;
    QByteArray layout_data;

    bool ok = file.getChar((char*)&geo_size);
    if (ok) {
        geo_data = file.read(geo_size);
        ok = geo_data.size() == geo_size;
    }
    if (ok) {
        layout_data = file.readAll();
        ok = layout_data.size() > 0;
    }

    if (ok)
        ok = restoreGeometry(geo_data);
    if (ok)
        ok = restoreState(layout_data);

    if (!ok) {
        QString msg = tr("Error reading %1")
                        .arg(fileName);
        qWarning() << tr("Error") << msg;
        return;
    }
}

void MainWindow::setEditBackgroundColor(const QColor &acolor)
{
    // set background color
    setAutoFillBackground(true);
    QPalette p = this->palette();
    p.setColor(QPalette::Base, acolor);
    textEdit->setPalette(p);
}

void MainWindow::openLastOpenedFiles()
{
    QStringList openFileList = mUtils->readStringList(OPENEDFILELIST);
    if (openFileList.size()){
        loadFile(openFileList.at(0));
    }
    else{
        setCurrentFile("");
    }
}

void MainWindow::snapshotOpenedFiles()
{
    if (curFile.isEmpty()){
        mUtils->removeConfig(OPENEDFILELIST);
    }
    else{
        QStringList openFileList(curFile);
        mUtils->writeStringList(OPENEDFILELIST, openFileList);

        closeFile(curFile);
    }
}

void MainWindow::newFile()
{
    if (maybeSave()) {
        textEdit->clear();
        setCurrentFile("");
    }
}

void MainWindow::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(this);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

void MainWindow::close()
{
    if (maybeSave()) {
        closeFile(curFile);
    }
    else{
        qWarning() << "DO NOT CLOSE FILE, as user click Cancel, or save failed.";
    }
}

bool MainWindow::rename()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.exec();
    QStringList files = dialog.selectedFiles();

    if (files.isEmpty()){
        qWarning() << "select nothing after QFileDialog.";
        return false;
    }

    return renameFile(files.at(0));
}

void MainWindow::remove()
{
    if (maybeSave()) {
        removeFile(curFile);
    }
    else{
        qWarning() << "DO NOT REMOVE FILE, as user click Cancel, or save failed.";
    }
}

bool MainWindow::save()
{
    if (curFile.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(curFile);
    }
}

void MainWindow::getEncoding()
{
    if (!curFile.isEmpty()) {
        getFileInfo(curFile);
        loadFile(curFile, mCodec, mHasBOM);
    }
}

void MainWindow::showInEncoding()
{
    if (curEncodingAct != NULL){
        if (curEncodingAct->isChecked()){
            // still checked, means user click another action, set this one checked false
            curEncodingAct->setChecked(false);
        }
        else {
            // the checked from true to false, means user click the same one, do nothing but set it checked back
            curEncodingAct->setChecked(true);
            return;
        }
    }

    if (encodeInANSIAct->isChecked())
        loadFile(curFile, QTextCodec::codecForLocale());
    else if (encodeInUTF8Act->isChecked())
        loadFile(curFile, QTextCodec::codecForName("UTF-8"), true);
    else if (encodeInUTF8WOBAct->isChecked())
        loadFile(curFile, QTextCodec::codecForName("UTF-8"));
    else if (encodeInUCS2BEAct->isChecked())
        loadFile(curFile, QTextCodec::codecForName("UTF-16BE"));
    else if (encodeInUCS2LEAct->isChecked())
        loadFile(curFile, QTextCodec::codecForName("UTF-16LE"));
    else{
        if (curEncodingAct != NULL){
            // as no other action checked, set curEncodingAct checked back
            curEncodingAct->setChecked(true);
        }
        QMessageBox::warning(this, Utils::getAppName(), tr("Not support right now."));
    }
}

void MainWindow::undoAll()
{
    while(textEdit->document()->isUndoAvailable())
        textEdit->document()->undo();
}

void MainWindow::redoAll()
{
    while(textEdit->document()->isRedoAvailable())
        textEdit->document()->redo();
}

void MainWindow::convertToEncoding()
{
    mHasBOM = false;
    textEdit->document()->setModified(true);
    setWindowModified(true);

    if (convertToANSIAct->isChecked()){
        convertToANSIAct->setChecked(false);
        mCodec = QTextCodec::codecForLocale();
    }
    else if (convertToUTF8Act->isChecked()){
        convertToUTF8Act->setChecked(false);
        mCodec = QTextCodec::codecForName("UTF-8");
        mHasBOM = true;
    }
    else if (convertToUTF8WOBAct->isChecked()){
        convertToUTF8WOBAct->setChecked(false);
        mCodec = QTextCodec::codecForName("UTF-8");
    }
    else if (convertToUCS2BEAct->isChecked()){
        convertToUCS2BEAct->setChecked(false);
        mCodec = QTextCodec::codecForName("UTF-16BE");
    }
    else if (convertToUCS2LEAct->isChecked()){
        convertToUCS2LEAct->setChecked(false);
        mCodec = QTextCodec::codecForName("UTF-16LE");
    }
    else{
        QMessageBox::warning(this, Utils::getAppName(), tr("Couldn't be here."));
        return;
    }

    setEncodingIcon(mCodec, mHasBOM);
}

void MainWindow::convertToEOL()
{
    textEdit->document()->setModified(true);
    setWindowModified(true);

    if (curEOLAct != NULL){
        if (curEOLAct->isChecked()){
            // still checked, means user click another action, set this one checked false
            curEOLAct->setChecked(false);
        }
        else {
            // the checked from true to false, means user click the same one, do nothing but set it checked back
            curEOLAct->setChecked(true);
            statusBar()->showMessage(tr("Item is already checked."), 2000);
            return;
        }
    }

    if (convertToWinAct->isChecked())
    {
        curEOLAct = convertToWinAct;
    }
    else if (convertToUnixAct->isChecked())
    {
        curEOLAct = convertToUnixAct;
    }
    else if (convertToMacAct->isChecked())
    {
        curEOLAct = convertToMacAct;
    }
    else{
        if (curEOLAct != NULL){
            // as no other action checked, set curEOLAct checked back
            curEOLAct->setChecked(true);
        }
        QMessageBox::warning(this, Utils::getAppName(), tr("Couldn't be here."));
    }
}

void MainWindow::enableEncodingAcion(bool enabled)
{
    getEncodingAct->setEnabled(enabled);

    encodeInANSIAct->setEnabled(enabled);
    encodeInUTF8WOBAct->setEnabled(enabled);
    encodeInUTF8Act->setEnabled(enabled);
    encodeInUCS2BEAct->setEnabled(enabled);
    encodeInUCS2LEAct->setEnabled(enabled);

    convertToANSIAct->setEnabled(enabled);
    convertToUTF8WOBAct->setEnabled(enabled);
    convertToUTF8Act->setEnabled(enabled);
    convertToUCS2BEAct->setEnabled(enabled);
    convertToUCS2LEAct->setEnabled(enabled);
}

void MainWindow::enableProjectAcion(bool enabled)
{
    removeProjAct->setEnabled(enabled);
    manageProjFileAct->setEnabled(enabled);
    syncProjAct->setEnabled(enabled);
    rebuildProjAct->setEnabled(enabled);
}

bool MainWindow::saveAs()
{
    QFileDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.exec();
    QStringList files = dialog.selectedFiles();

    if (files.isEmpty()){
        qWarning() << "select nothing after QFileDialog.";
        return false;
    }

    return saveFile(files.at(0));
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Application"),
            tr("The <b>Application</b> example demonstrates how to "
               "write modern GUI applications using Qt, with a menu bar, "
               "toolbars, and a status bar."));
}

void MainWindow::documentWasModified()
{
    setWindowModified(textEdit->document()->isModified());
}

void MainWindow::openSelectFile(const QString & fileName)
{
    if(curFile == fileName)
        return;

    loadFile(fileName);
}

void MainWindow::newProject()
{
    if (!mProjName.isEmpty()){
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, NULL,
                     tr("OK to Close current project:\"%1\"?").arg(mProjName),
                     QMessageBox::Ok | QMessageBox::Cancel);
        if (ret == QMessageBox::Cancel){
            return;
        }
        else{
            closeProject();
        }
    }

    NewProjectDialog newProjectDialog(this);
    newProjectDialog.setWindowModality(Qt::WindowModal);
    if (newProjectDialog.exec() == QDialog::Rejected){
        qDebug() << "NewProjectDialog is Rejected.";
        return;
    }

    mProjName = newProjectDialog.getProjName();
    mProjStorePath = newProjectDialog.getProjStorePath();
    QString srcRootPath = newProjectDialog.getSrcRootPath();

    AddFilesDialog addFilesDialog(mProjName, mProjStorePath, srcRootPath, this);
    addFilesDialog.setWindowModality(Qt::WindowModal);
    if (addFilesDialog.exec() == QDialog::Rejected){
        qDebug() << "AddFilesDialog is Rejected.";
        return;
    }

    mProjectWindow->updateFileList();
    enableProjectAcion(true);
    openLastOpenedFiles();
}

void MainWindow::openProject()
{
    if (!mProjName.isEmpty()){
        closeProject();
    }

    OpenProjectDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    if (dialog.exec() == QDialog::Rejected){
        qDebug() << "NewProjectDialog is Rejected.";
        return;
    }

    mProjName = dialog.getProjName();
    mProjStorePath = dialog.getProjStorePath();

    mProjectWindow->updateFileList();
    enableProjectAcion(true);
    openLastOpenedFiles();
}

void MainWindow::removeProject()
{
    RemoveProjectDialog dialog(this);
    dialog.setWindowModality(Qt::WindowModal);
    if (dialog.exec() == QDialog::Rejected){
        qDebug() << "RemoveProjectDialog is Rejected.";
        return;
    }

    mProjName = dialog.getProjName();
    if (mProjName == dialog.getProjName()){
        mProjName.clear();
        mProjStorePath.clear();
        mUtils->setCurrentProject(mProjName, mProjStorePath);
        mProjectWindow->updateFileList();
        enableProjectAcion(false);
    }

    // FIXME: add a function to close all file, and invoked here and snapshotOpenedFiles()
    if (curFile.isEmpty()){
        mUtils->removeConfig(OPENEDFILELIST);
    }
    else{
        closeFile(curFile);
    }
}

void MainWindow::closeProject()
{
    if (mProjStorePath.isEmpty() || mProjName.isEmpty())
        return;

    snapshotOpenedFiles();

    mProjName.clear();
    mProjStorePath.clear();
    mUtils->setCurrentProject(mProjName, mProjStorePath);
    mProjectWindow->updateFileList();
    enableProjectAcion(false);
}

void MainWindow::manageProjFile()
{
    QString srcRootPath = mUtils->readString(SRCROOTPATH);

    AddFilesDialog addFilesDialog(mProjName, mProjStorePath, srcRootPath, this);
    addFilesDialog.setWindowModality(Qt::WindowModal);
    if (addFilesDialog.exec() == QDialog::Rejected){
        qDebug() << "AddFilesDialog is Rejected.";
        return;
    }

    mProjectWindow->updateFileList();
}

void MainWindow::createActions()
{
    newAct = new QAction(QIcon(":/images/new.png"), tr("&New"), this);
    newAct->setShortcuts(QKeySequence::New);
    newAct->setStatusTip(tr("Create a new file"));
    connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

    closeAct = new QAction(tr("&Close"), this);
    closeAct->setShortcuts(QKeySequence::listFromString(tr("Ctrl+W")));
    closeAct->setStatusTip(tr("Closes the current file."));
    connect(closeAct, SIGNAL(triggered()), this, SLOT(close()));

    renameAct = new QAction(tr("Rename..."), this);
    renameAct->setStatusTip(tr("Renames a file on disk and in a project."));
    connect(renameAct, SIGNAL(triggered()), this, SLOT(rename()));

    removeAct = new QAction(tr("&Delete File..."), this);
    removeAct->setStatusTip(tr("Deletes a file on disk and in a project."));
    connect(removeAct, SIGNAL(triggered()), this, SLOT(remove()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::listFromString(tr("Ctrl+Shift+S")));
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    undoAct = new QAction(tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Reverses the last editing operation."));
    connect(undoAct, SIGNAL(triggered()), textEdit, SLOT(undo()));
    undoAct->setEnabled(false);

    redoAct = new QAction(tr("&Redo"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("Recreates the last editing operation after an Undo command."));
    connect(redoAct, SIGNAL(triggered()), textEdit, SLOT(redo()));
    redoAct->setEnabled(false);

    undoAllAct = new QAction(tr("U&ndo All"), this);
    undoAllAct->setStatusTip(tr("Undoes all edits to the current file."));
    connect(undoAllAct, SIGNAL(triggered()), this, SLOT(undoAll()));
    undoAllAct->setEnabled(false);

    redoAllAct = new QAction(tr("R&edo All"), this);
    redoAllAct->setStatusTip(tr("Redoes all edits to the current file."));
    connect(redoAllAct, SIGNAL(triggered()), this, SLOT(redoAll()));
    redoAllAct->setEnabled(false);

    cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the "
                            "clipboard"));
    connect(cutAct, SIGNAL(triggered()), textEdit, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), textEdit, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), textEdit, SLOT(paste()));

    convertToWinAct = new QAction(tr("Convert to Windows Format"), this);
    convertToWinAct->setCheckable(true);
    connect(convertToWinAct, SIGNAL(triggered()), this, SLOT(convertToEOL()));

    convertToUnixAct = new QAction(tr("Convert to UNIX Format"), this);
    convertToUnixAct->setCheckable(true);
    connect(convertToUnixAct, SIGNAL(triggered()), this, SLOT(convertToEOL()));

    convertToMacAct = new QAction(tr("Convert to MAC Format"), this);
    convertToMacAct->setCheckable(true);
    connect(convertToMacAct, SIGNAL(triggered()), this, SLOT(convertToEOL()));

    getEncodingAct = new QAction(tr("Get encoding"), this);
    connect(getEncodingAct, SIGNAL(triggered()), this, SLOT(getEncoding()));

    encodeInANSIAct = new QAction(tr("Encode in ANSI"), this);
    encodeInANSIAct->setCheckable(true);
    connect(encodeInANSIAct, SIGNAL(triggered()), this, SLOT(showInEncoding()));

    encodeInUTF8WOBAct = new QAction(tr("Encode in UTF-8 without BOM"), this);
    encodeInUTF8WOBAct->setCheckable(true);
    connect(encodeInUTF8WOBAct, SIGNAL(triggered()), this, SLOT(showInEncoding()));

    encodeInUTF8Act = new QAction(tr("Encode in UTF-8"), this);
    encodeInUTF8Act->setCheckable(true);
    connect(encodeInUTF8Act, SIGNAL(triggered()), this, SLOT(showInEncoding()));

    encodeInUCS2BEAct = new QAction(tr("Encode in UCS-2 Big Endian"), this);
    encodeInUCS2BEAct->setCheckable(true);
    connect(encodeInUCS2BEAct, SIGNAL(triggered()), this, SLOT(showInEncoding()));

    encodeInUCS2LEAct = new QAction(tr("Encode in UCS-2 Little Endian"), this);
    encodeInUCS2LEAct->setCheckable(true);
    connect(encodeInUCS2LEAct, SIGNAL(triggered()), this, SLOT(showInEncoding()));

    convertToANSIAct = new QAction(tr("Convert to ANSI"), this);
    convertToANSIAct->setCheckable(true);
    connect(convertToANSIAct, SIGNAL(triggered()), this, SLOT(convertToEncoding()));

    convertToUTF8WOBAct = new QAction(tr("Convert to UTF-8 without BOM"), this);
    convertToUTF8WOBAct->setCheckable(true);
    connect(convertToUTF8WOBAct, SIGNAL(triggered()), this, SLOT(convertToEncoding()));

    convertToUTF8Act = new QAction(tr("Convert to UTF-8"), this);
    convertToUTF8Act->setCheckable(true);
    connect(convertToUTF8Act, SIGNAL(triggered()), this, SLOT(convertToEncoding()));

    convertToUCS2BEAct = new QAction(tr("Convert to UCS-2 Big Endian"), this);
    convertToUCS2BEAct->setCheckable(true);
    connect(convertToUCS2BEAct, SIGNAL(triggered()), this, SLOT(convertToEncoding()));

    convertToUCS2LEAct = new QAction(tr("Convert to UCS-2 Little Endian"), this);
    convertToUCS2LEAct->setCheckable(true);
    connect(convertToUCS2LEAct, SIGNAL(triggered()), this, SLOT(convertToEncoding()));

    newProjAct = new QAction(tr("&New Project..."), this);
    newProjAct->setShortcuts(QKeySequence::listFromString(tr("Alt+Shift+N")));
    newProjAct->setStatusTip(tr("Creates a new project."));
    connect(newProjAct, SIGNAL(triggered()), this, SLOT(newProject()));

    openProjAct = new QAction(tr("&Open Project..."), this);
    openProjAct->setShortcuts(QKeySequence::listFromString(tr("Alt+Shift+P")));
    openProjAct->setStatusTip(tr("Opens an existing project."));
    connect(openProjAct, SIGNAL(triggered()), this, SLOT(openProject()));

    closeProjAct = new QAction(tr("&Close Project"), this);
    closeProjAct->setShortcuts(QKeySequence::listFromString(tr("Alt+Shift+W")));
    closeProjAct->setStatusTip(tr("Closes the current project."));
    connect(closeProjAct, SIGNAL(triggered()), this, SLOT(closeProject()));

    removeProjAct = new QAction(tr("&Remove Project..."), this);
    removeProjAct->setStatusTip(tr("Deletes a project."));
    connect(removeProjAct, SIGNAL(triggered()), this, SLOT(removeProject()));

    manageProjFileAct = new QAction(tr("&Add and Remove Project Files..."), this);
    manageProjFileAct->setStatusTip(tr("Adds and removes files from current project."));
    connect(manageProjFileAct, SIGNAL(triggered()), this, SLOT(manageProjFile()));

    syncProjAct = new QAction(tr("&Synchronize Files..."), this);
    syncProjAct->setShortcuts(QKeySequence::listFromString(tr("Alt+Shift+S")));
    syncProjAct->setStatusTip(tr("Synchronizes the current project with changes made to files outside the editor."));

    rebuildProjAct = new QAction(tr("R&ebuild Project..."), this);
    rebuildProjAct->setStatusTip(tr("Rebuilds project date files."));

    projSettingAct = new QAction(tr("Pro&ject Settings..."), this);
    projSettingAct->setStatusTip(tr("Specifies options for Project."));

    enableProjectAcion(false);

    aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("Show the application's About box"));
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    aboutQtAct->setStatusTip(tr("Show the Qt library's About box"));
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    saveAct->setEnabled(false);
    cutAct->setEnabled(false);
    copyAct->setEnabled(false);
    connect(textEdit, SIGNAL(modificationChanged(bool)),
            saveAct, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(copyAvailable(bool)),
            cutAct, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(copyAvailable(bool)),
            copyAct, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(undoAvailable(bool)),
            undoAct, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(redoAvailable(bool)),
            redoAct, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(undoAvailable(bool)),
            undoAllAct, SLOT(setEnabled(bool)));
    connect(textEdit, SIGNAL(redoAvailable(bool)),
            redoAllAct, SLOT(setEnabled(bool)));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(newAct);
    fileMenu->addAction(openAct);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAct);
    fileMenu->addAction(renameAct);
    fileMenu->addAction(removeAct);
    fileMenu->addSeparator();
    fileMenu->addAction(saveAct);
    fileMenu->addAction(saveAsAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    editMenu = menuBar()->addMenu(tr("&Edit"));
    editMenu->addAction(undoAct);
    editMenu->addAction(redoAct);
    editMenu->addAction(undoAllAct);
    editMenu->addAction(redoAllAct);
    editMenu->addSeparator();
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);
    editMenu->addSeparator();
    eolConvMenu = editMenu->addMenu(tr("EOL Conversion"));
    eolConvMenu->addAction(convertToWinAct);
    eolConvMenu->addAction(convertToUnixAct);
    eolConvMenu->addAction(convertToMacAct);

    encodingMenu = menuBar()->addMenu(tr("E&ncoding"));
    encodingMenu->addAction(getEncodingAct);
    encodingMenu->addAction(encodeInANSIAct);
    encodingMenu->addAction(encodeInUTF8WOBAct);
    encodingMenu->addAction(encodeInUTF8Act);
    encodingMenu->addAction(encodeInUCS2BEAct);
    encodingMenu->addAction(encodeInUCS2LEAct);
    encodingMenu->addSeparator();
    encodingMenu->addAction(convertToANSIAct);
    encodingMenu->addAction(convertToUTF8WOBAct);
    encodingMenu->addAction(convertToUTF8Act);
    encodingMenu->addAction(convertToUCS2BEAct);
    encodingMenu->addAction(convertToUCS2LEAct);

    projectMenu = menuBar()->addMenu(tr("&Project"));
    projectMenu->addAction(newProjAct);
    projectMenu->addAction(openProjAct);
    projectMenu->addAction(removeProjAct);
    projectMenu->addAction(closeProjAct);
    projectMenu->addSeparator();
    projectMenu->addAction(manageProjFileAct);
    projectMenu->addAction(syncProjAct);
    projectMenu->addSeparator();
    projectMenu->addAction(rebuildProjAct);
    projectMenu->addSeparator();
    projectMenu->addAction(projSettingAct);

    viewMenu = menuBar()->addMenu(tr("&View"));

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->setObjectName("SV_ToolBar_File");
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->setObjectName("SV_ToolBar_Edit");
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
}

void MainWindow::setupDockWidgets()
{
    mProjectWindow = new FileListFrame(tr("projectWindow"), this);
    addDockWidget(Qt::RightDockWidgetArea, mProjectWindow);
    viewMenu->addAction(mProjectWindow->toggleViewAction());
    connect(mProjectWindow, SIGNAL(onFileSelected(const QString &)),
            this, SLOT(openSelectFile(const QString &)));

    mSymbolWindow = new WindowSwatch(tr("symbolWindow"), this);
    addDockWidget(Qt::LeftDockWidgetArea, mSymbolWindow);
    viewMenu->addAction(mSymbolWindow->toggleViewAction());

    mContextWindows = new WindowSwatch(tr("contextWindows"), this);
    addDockWidget(Qt::BottomDockWidgetArea, mContextWindows);
    viewMenu->addAction(mContextWindows->toggleViewAction());

    mRelationWindow = new WindowSwatch(tr("relationWindow"), this);
    addDockWidget(Qt::BottomDockWidgetArea, mRelationWindow);
    viewMenu->addAction(mRelationWindow->toggleViewAction());
}

void MainWindow::readSettings()
{
    QSettings settings("QtProject", "Application Example");
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

void MainWindow::writeSettings()
{
    QSettings settings("QtProject", "Application Example");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

bool MainWindow::maybeSave()
{
    if (textEdit->document()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Application"),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel){
            qWarning() << "user click Cancel, do nothing.";
            return false;
        }
        else{ //QMessageBox::Discard
            qWarning() << "user click Discard, no need save file.";
            return true;
        }
    }
    return true;
}

void MainWindow::loadFile(const QString &fileName, QTextCodec *codec, bool hasBOM)
{
    // creat a new QString, if fileName is same with curFile
    // after closeFile(curFile), we can still reopen it
    QString newFileName(fileName);

    //close current file if needed
    if(!curFile.isEmpty())
        closeFile(curFile);

    QFile file(newFileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(newFileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);

    if(NULL != codec){
        mCodec = codec;
        mHasBOM = hasBOM;
    }

    if(NULL != mCodec){
        in.setCodec(mCodec);
    }

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    textEdit->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(newFileName);
    setEncodingIcon(mCodec, mHasBOM);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

void MainWindow::closeFile(const QString &fileName)
{
    if(fileName.isEmpty())
        return;

//    qWarning() << "close file : " << fileName;
    QFile file(fileName);
    file.close();

    mCodec = NULL;
    mHasBOM = false;
    textEdit->setPlainText("");

    setCurrentFile("");
    setEncodingIcon(NULL);
    if(curEOLAct){
        curEOLAct->setChecked(false);
        curEOLAct = NULL;
    }
    statusBar()->showMessage(tr("File closed"), 2000);
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, Utils::getAppName(),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        qWarning() << tr("Cannot write file %1:\n%2.").arg(fileName).arg(file.errorString());
        return false;
    }

    QTextStream out(&file);
    if(NULL != mCodec){
        out.setCodec(mCodec);
        if(mHasBOM && mCodec->name() == "UTF-8"){
            QDataStream dataStream(&file);
            char bom[3] = {(char)0xEF, (char)0xBB, (char)0xBF};
            dataStream.writeRawData(bom, sizeof(bom));
            qDebug() << "write UTF-8 with BOM:0xEF BB BF";
        }
        else if (mCodec->name() == "UTF-16BE"){
            QDataStream dataStream(&file);
            char bom[2] = {(char)0xFE, (char)0xFF};
            dataStream.writeRawData(bom, sizeof(bom));
            qDebug() << "write UCS-2 Big Endian with BOM:0xFE FF.";
        }
        else if (mCodec->name() == "UTF-16LE"){
            QDataStream dataStream(&file);
            char bom[2] = {(char)0xFF, (char)0xFE};
            dataStream.writeRawData(bom, sizeof(bom));
            qDebug() << "write UCS-2 Little Endian with BOM:0xFF FE.";
        }
        else {
            qDebug() << "use default encoding(GBK) write to file.";
        }
    }
#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    out << textEdit->toPlainText();
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

bool MainWindow::renameFile(const QString &fileName)
{
    QFile file(curFile);
    if (!file.rename(fileName)) {
        QMessageBox::warning(this, Utils::getAppName(),
                             tr("Cannot rename file %1 to %2:\n%3.")
                             .arg(curFile)
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File renamed"), 2000);
    return true;
}

bool MainWindow::removeFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.remove(fileName)) {
        QMessageBox::warning(this, Utils::getAppName(),
                             tr("Cannot delete file %1:\n%3.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    textEdit->setPlainText("");
    setCurrentFile("");
    statusBar()->showMessage(tr("File deleted"), 2000);
    return true;
}

void MainWindow::setCurrentFile(const QString &fileName)
{
    curFile = fileName;
    textEdit->document()->setModified(false);
    setWindowModified(false);

    QString shownName = fileName;
    if (fileName.isEmpty()){
        textEdit->setReadOnly(true);
        setEditBackgroundColor(QColor("#C7C7C7"));
        shownName = "untitled";
        renameAct->setEnabled(false);
        removeAct->setEnabled(false);
        enableEncodingAcion(false);
        eolConvMenu->setEnabled(false);
    }
    else{
        textEdit->setReadOnly(false);
        setEditBackgroundColor(QColor("#FFFFFF"));
        renameAct->setEnabled(true);
        removeAct->setEnabled(true);
        enableEncodingAcion(true);
        eolConvMenu->setEnabled(true);
    }

    QString shownTitle = "[*]";
    if (!mProjName.isEmpty()){
        shownTitle += mProjName;
        shownTitle += " Project";
    }
    else
        shownTitle += "(No Project)";

    if (!fileName.isEmpty()){
        shownTitle += " - [ " + fileName + " ]";
    }

    setWindowTitle(shownTitle);
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::getFileInfo(const QString &fileName)
{
    QProcess p(0);
 #if defined (Q_OS_WIN32)
    QString command = "windows\\file\\file.exe -m windows\\file\\magic \"" + fileName + "\"";
    p.start(command);
    qDebug() << "works on WIN32 platform, command : " << command;
#else
    p.start("file \""+fileName+"\"");
#endif
    p.waitForStarted();
    p.waitForFinished();
    QString fileInfo = QString::fromLocal8Bit(p.readAllStandardOutput());
//    qDebug() << "command result: " << fileInfo;

    QString codecInfo = fileInfo.remove(fileName).remove(": ");
    qDebug() << codecInfo;

    if(codecInfo.contains("UTF-8 Unicode")){
        mCodec = QTextCodec::codecForName("UTF-8");
        if(codecInfo.contains("with BOM")){
            mHasBOM = true;
        }
    }
    else if(codecInfo.contains("ISO-8859")){
        mCodec = QTextCodec::codecForLocale(); //use local codec, show as ANSI
    }
    else if(codecInfo.contains("ASCII")){
        mCodec = QTextCodec::codecForName("ASCII");
    }
    else if(codecInfo.contains("UTF-16 Unicode")){
        if (codecInfo.contains("Big-endian")){
            mCodec = QTextCodec::codecForName("UTF-16BE");
        }
        else if (codecInfo.contains("Little-endian")){
            mCodec = QTextCodec::codecForName("UTF-16LE");
        }
        else {
            mCodec = QTextCodec::codecForName("UTF-16");
        }
    }
    else{
        qWarning() << "unknown encoding : " << codecInfo;
    }
}

void MainWindow::setEncodingIcon(const QTextCodec * codec, bool hasBOM)
{
    // hide last encoding action icon
    if(curEncodingAct){
        curEncodingAct->setChecked(false);
        curEncodingAct = NULL;
    }

    if(NULL == codec)
        return;

    if(codec->name() == "UTF-8"){
        if(hasBOM)
            curEncodingAct = encodeInUTF8Act;
        else
            curEncodingAct = encodeInUTF8WOBAct;
    }
    else if(codec->name() == "GBK"){
        curEncodingAct = encodeInANSIAct;
    }
    else if(codec->name() == "UTF-16BE"){
        curEncodingAct = encodeInUCS2BEAct;
    }
    else if(codec->name() == "UTF-16LE"){
        curEncodingAct = encodeInUCS2LEAct;
    }
    else{
        qWarning() << "[setEncodingIcon] unknown encoding : " << codec;
    }

    if(curEncodingAct){
        curEncodingAct->setChecked(true);
    }
}
