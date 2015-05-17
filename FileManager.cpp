
#include "FileManager.h"

#define CHECK_EDITOR() \
    if (NULL == currentEditor()) \
        return;

FileManager::FileManager(QMainWindow * window) :
                mWindow(window),
                mEditor(NULL),
                mCurFileName(),
                mNewFileIndex(1)
{
    mStackedWidget = new QStackedWidget;
}

FileManager::~FileManager()
{
    while (mStackedWidget->count() > 0){
        CodeEditor * editor = (CodeEditor *)mStackedWidget->currentWidget();
        delete editor;
    }
    delete mStackedWidget;
}

void FileManager::newFile()
{
    CodeEditor * editor = new CodeEditor();
    mStackedWidget->addWidget(editor);
    setCurrentFile("");
}

void FileManager::open()
{
    QString fileName = QFileDialog::getOpenFileName(mWindow);
    if (!fileName.isEmpty())
        loadFile(fileName);
}

void FileManager::close()
{
    if (maybeSave()) {
        closeFile(mCurFileName);
    }
    else{
        qWarning() << "DO NOT CLOSE FILE, as user click Cancel, or save failed.";
    }
}

bool FileManager::rename()
{
    QFileDialog dialog(mWindow);
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

void FileManager::remove()
{
    if (maybeSave()) {
        removeFile(mCurFileName);
    }
    else{
        qWarning() << "DO NOT REMOVE FILE, as user click Cancel, or save failed.";
    }
}

bool FileManager::save()
{
    if (mCurFileName.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(mCurFileName);
    }
}

bool FileManager::saveAs()
{
    QFileDialog dialog(mWindow);
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
    if(mCurFileName == fileName)
        return;

    loadFile(fileName);
}

bool FileManager::maybeSave()
{
    if (mEditor->document()->isModified()) {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(mWindow, tr("Application"),
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

void FileManager::loadFile(const QString &fileName)
{
    // creat a new QString, if fileName is same with mCurFileName
    // after closeFile(mCurFileName), we can still reopen it
    QString newFileName(fileName);

    //close current file if needed
    if(!mCurFileName.isEmpty())
        closeFile(mCurFileName);

    QFile file(newFileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(mWindow, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(newFileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);

    QApplication::setOverrideCursor(Qt::WaitCursor);
    mEditor->setPlainText(in.readAll());
    QApplication::restoreOverrideCursor();

    setCurrentFile(newFileName);
    mWindow->statusBar()->showMessage(tr("File loaded"), 2000);
}

bool FileManager::saveFile(const QString &fileName)
{
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
    out << mEditor->toPlainText();
    QApplication::restoreOverrideCursor();

    setCurrentFile(fileName);
    mWindow->statusBar()->showMessage(tr("File saved"), 2000);
    return true;
}

bool FileManager::renameFile(const QString &fileName)
{
    QFile file(mCurFileName);
    if (!file.rename(fileName)) {
        QMessageBox::warning(mWindow, Utils::getAppName(),
                             tr("Cannot rename file %1 to %2:\n%3.")
                             .arg(mCurFileName)
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    setCurrentFile(fileName);
    mWindow->statusBar()->showMessage(tr("File renamed"), 2000);
    return true;
}

bool FileManager::removeFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.remove(fileName)) {
        QMessageBox::warning(mWindow, Utils::getAppName(),
                             tr("Cannot delete file %1:\n%3.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return false;
    }

    mEditor->clear();
    setCurrentFile("");
    mWindow->statusBar()->showMessage(tr("File deleted"), 2000);
    return true;
}

void FileManager::closeFile(const QString &fileName)
{
    if(fileName.isEmpty())
        return;

    QFile file(fileName);
    file.close();

    mEditor->clear();

    setCurrentFile("");

    mWindow->statusBar()->showMessage(tr("File closed"), 2000);
}

void FileManager::setCurrentFile(const QString &fileName)
{
    CodeEditor * editor = currentEditor();
    disconnect(editor->document(), SIGNAL(contentsChanged()), 0, 0);// disconnect last SIGNAL
    connect(editor->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));
    mWindow->setWindowModified(false);

    QString shownTitle = "(No Project)";
    QString projName = Utils::enstance()->getCurProjName();
    if (!projName.isEmpty()){
        shownTitle = projName + " Project";
    }

    if (fileName.isEmpty())
        mCurFileName = "new " + QString::number(mNewFileIndex++);
    else
        mCurFileName = fileName;
    shownTitle += " - [ " + mCurFileName + "[*]]";

    mWindow->setWindowTitle(shownTitle);
}
