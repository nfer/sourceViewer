
#include "FileManager.h"

#define CHECK_EDITOR() \
    if (NULL == currentEditor()) \
        return;

#define CHECK_EDITOR_BOOL() \
    if (NULL == currentEditor()) \
        return false;

FileManager::FileManager(QMainWindow * window) :
                mWindow(window),
                mNewFileIndex(1),
                mFileNameHash()
{
    mTabWidget = new QTabWidget;
    mTabWidget->setMovable(true);
    mTabWidget->setTabsClosable(true);
    mTabWidget->installEventFilter(this);

    connect(mTabWidget,SIGNAL(tabBarDoubleClicked(int)),this,SLOT(removeSubTab(int)));
    connect(mTabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(removeSubTab(int)));
    connect(mTabWidget,SIGNAL(currentChanged(int)),this,SLOT(curTabChanged(int)));
}

FileManager::~FileManager()
{
    while (mTabWidget->count() > 0){
        CodeEditor * editor = (CodeEditor *)mTabWidget->currentWidget();
        delete editor;
    }
    delete mTabWidget;
}

void FileManager::newFile()
{
    CodeEditor * editor = new CodeEditor();
    QString fileName = "new " + QString::number(mNewFileIndex++);
    mFileNameHash.insert(editor, fileName);
    mTabWidget->addTab(editor, fileName);
    mTabWidget->setCurrentWidget(editor);
}

void FileManager::open()
{
    QString fileName = QFileDialog::getOpenFileName(mWindow);
    if (!fileName.isEmpty())
        openSelectFile(fileName);
}

void FileManager::close()
{
    removeSubTab(mTabWidget->currentIndex());
}

bool FileManager::rename()
{
    CHECK_EDITOR_BOOL();
    QFileDialog dialog(mWindow);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.exec();
    QStringList files = dialog.selectedFiles();

    if (files.isEmpty()){
        qDebug() << "select nothing after QFileDialog.";
        return false;
    }

    return renameFile(files.at(0));
}

void FileManager::remove()
{
    CHECK_EDITOR();
    if (maybeSave()) {
        removeFile();
    }
    else{
        qDebug() << "DO NOT REMOVE FILE, as user click Cancel, or save failed.";
    }
}

bool FileManager::save()
{
    CHECK_EDITOR_BOOL();
    QString fileName = mFileNameHash.value(currentEditor());
    qDebug() << "currentEditor attached file name is " << fileName;
    if (fileName.startsWith("new ")) {
        return saveAs();
    } else {
        return saveFile(fileName);
    }
}

bool FileManager::saveAs()
{
    CHECK_EDITOR_BOOL();
    QFileDialog dialog(mWindow);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAcceptMode(QFileDialog::AcceptSave);
    dialog.exec();
    QStringList files = dialog.selectedFiles();

    if (files.isEmpty()){
        qDebug() << "select nothing after QFileDialog.";
        return false;
    }

    return saveFile(files.at(0));
}

void FileManager::undo()
{
    CHECK_EDITOR();
    currentEditor()->undo();
}

void FileManager::redo()
{
    CHECK_EDITOR();
    currentEditor()->redo();
}

void FileManager::undoAll()
{
    CHECK_EDITOR();
    while(currentEditor()->document()->isUndoAvailable())
        currentEditor()->undo();
}

void FileManager::redoAll()
{
    CHECK_EDITOR();
    while(currentEditor()->document()->isRedoAvailable())
        currentEditor()->redo();
}

void FileManager::cut()
{
    CHECK_EDITOR();
    currentEditor()->cut();
}

void FileManager::copy()
{
    CHECK_EDITOR();
    currentEditor()->copy();
}

void FileManager::paste()
{
    CHECK_EDITOR();
    currentEditor()->paste();
}

void FileManager::documentWasModified()
{
    CHECK_EDITOR();
    mWindow->setWindowModified(currentEditor()->document()->isModified());
}

void FileManager::openSelectFile(const QString & fileName)
{
    loadFile(fileName);
    mTabWidget->setCurrentIndex(mTabWidget->count());
}

void FileManager::openLastOpenedFiles()
{
    QStringList openedFiles = Utils::enstance()->readStringList(OPENEDFILELIST);
    for(int i=0; i<openedFiles.count(); i++){
        loadFile(openedFiles.at(i));
    }

    if (openedFiles.count() > 0)
        mTabWidget->setCurrentIndex(Utils::enstance()->readInt(CURTABINDEX));
}

void FileManager::snapshotOpenedFiles()
{
    Utils::enstance()->writeInt(CURTABINDEX, mTabWidget->currentIndex());

    QStringList openedFiles;
    while (mTabWidget->count()){
        CodeEditor * editor = (CodeEditor *)mTabWidget->widget(0);
        openedFiles += mFileNameHash.value(editor);
        mFileNameHash.remove(editor);
        delete editor;
    }
    Utils::enstance()->writeStringList(OPENEDFILELIST, openedFiles);
}

void FileManager::removeSubTab(int index)
{
    if (maybeSave()){
        CodeEditor * editor = (CodeEditor *)mTabWidget->widget(index);
        mFileNameHash.remove(editor);
        delete editor;
        showStatusBarMsg(tr("File closed"));
    }
}

void FileManager::curTabChanged(int index)
{
    QString fileName;
    if (-1 != index){
        fileName = mFileNameHash.value(currentEditor());
    }

    setCurrentFile(fileName);
}

void FileManager::showStatusBarMsg(const QString & message, int timeout)
{
    if (mWindow)
        mWindow->statusBar()->showMessage(message, timeout);
}

// FIXME: maybeSave not always save current editor
bool FileManager::maybeSave()
{
    CHECK_EDITOR_BOOL();
    if (currentEditor()->document()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(mWindow, tr("Application"),
                     tr("The document has been modified.\n"
                        "Do you want to save your changes?"),
                     QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        if (ret == QMessageBox::Save)
            return save();
        else if (ret == QMessageBox::Cancel){
            qDebug() << "user click Cancel, do nothing.";
            return false;
        }
        else{ //QMessageBox::Discard
            qDebug() << "user click Discard, no need save file.";
            return true;
        }
    }
    return true;
}

void FileManager::loadFile(const QString &fileName)
{
    // FIXME: should check whether already opened this file

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(mWindow, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QApplication::setOverrideCursor(Qt::WaitCursor);
    CodeEditor * editor = new CodeEditor();
    editor->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    // FIXME: whether we need close this file after load contents

    mFileNameHash.insert(editor, fileName);
    mTabWidget->addTab(editor, QFileInfo(fileName).fileName());

    showStatusBarMsg(tr("File loaded"));
}

bool FileManager::saveFile(const QString &fileName)
{
    CHECK_EDITOR_BOOL();
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(mWindow, Utils::getAppName(),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    QTextStream out(&file);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    out << currentEditor()->toPlainText();
    QApplication::restoreOverrideCursor();

    showStatusBarMsg(tr("File saved"));
    return true;
}

bool FileManager::renameFile(const QString &newFileName)
{
    QString oldFileName = mFileNameHash.value(currentEditor());
    QFile file(oldFileName);
    if (!file.rename(newFileName)) {
        QMessageBox::warning(mWindow, Utils::getAppName(),
                             tr("Cannot rename file %1 to %2:\n%3.")
                             .arg(oldFileName)
                             .arg(newFileName)
                             .arg(file.errorString()));
        return false;
    }

    mFileNameHash[currentEditor()] = newFileName;
    mTabWidget->setTabText(mTabWidget->currentIndex(), QFileInfo(newFileName).fileName());
    showStatusBarMsg(tr("File renamed"));
    return true;
}

bool FileManager::removeFile()
{
    CodeEditor * editor = currentEditor();
    QString fileName = mFileNameHash.value(editor);

    QFile file(fileName);
    if (!file.remove(fileName)) {
        QMessageBox::warning(mWindow, Utils::getAppName(),
                             tr("Cannot delete file %1:\n%3.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    mFileNameHash.remove(editor);
    delete editor;

    showStatusBarMsg(tr("File deleted"));
    return true;
}

void FileManager::setCurrentFile(const QString &fileName)
{
    QString shownTitle = "(No Project)";
    QString projName = Utils::enstance()->getCurProjName();
    if (!projName.isEmpty()){
        shownTitle = projName + " Project";
    }

    if (!fileName.isEmpty())
        shownTitle += " - [ " + fileName + " [*]]";

    mWindow->setWindowTitle(shownTitle);

    CHECK_EDITOR();
    QTextDocument * document = currentEditor()->document();
    disconnect(document, SIGNAL(contentsChanged()), 0, 0);// disconnect last SIGNAL
    connect(document, SIGNAL(contentsChanged()), this, SLOT(documentWasModified()));
    documentWasModified();
}

bool FileManager::eventFilter(QObject*obj,QEvent*event)
{
    if(event->type() == QEvent::MouseButtonDblClick && obj == mTabWidget)
    {
        newFile();
        return true;
    }
    else{
        return QObject::eventFilter(obj,event);
    }
}
