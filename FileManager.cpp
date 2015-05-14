
#include "FileManager.h"

FileManager::FileManager(QMainWindow * window) :
				mWindow(window),
				mFileName()
{
	mEditor = new CodeEditor;

    connect(mEditor->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));
}

FileManager::~FileManager()
{
    delete mEditor;
}

void FileManager::newFile()
{
    if (maybeSave()) {
        mEditor->clear();
        setCurrentFile("");
    }
}

void FileManager::open()
{
    if (maybeSave()) {
        QString fileName = QFileDialog::getOpenFileName(mWindow);
        if (!fileName.isEmpty())
            loadFile(fileName);
    }
}

void FileManager::close()
{
    if (maybeSave()) {
        closeFile(mFileName);
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
        removeFile(mFileName);
    }
    else{
        qWarning() << "DO NOT REMOVE FILE, as user click Cancel, or save failed.";
    }
}

bool FileManager::save()
{
    if (mFileName.isEmpty()) {
        return saveAs();
    } else {
        return saveFile(mFileName);
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
    mEditor->undo();
}

void FileManager::redo()
{
    mEditor->redo();
}

void FileManager::undoAll()
{
    while(mEditor->document()->isUndoAvailable())
        mEditor->undo();
}

void FileManager::redoAll()
{
    while(mEditor->document()->isRedoAvailable())
        mEditor->redo();
}

void FileManager::cut()
{
    mEditor->cut();
}

void FileManager::copy()
{
    mEditor->copy();
}

void FileManager::paste()
{
    mEditor->paste();
}

void FileManager::documentWasModified()
{
    if (!mFileName.isEmpty())
        mWindow->setWindowModified(mEditor->document()->isModified());
}

void FileManager::openSelectFile(const QString & fileName)
{
    if(mFileName == fileName)
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
    // creat a new QString, if fileName is same with mFileName
    // after closeFile(mFileName), we can still reopen it
    QString newFileName(fileName);

    //close current file if needed
    if(!mFileName.isEmpty())
        closeFile(mFileName);

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
    QFile file(mFileName);
    if (!file.rename(fileName)) {
        QMessageBox::warning(mWindow, Utils::getAppName(),
                             tr("Cannot rename file %1 to %2:\n%3.")
                             .arg(mFileName)
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
    mFileName = fileName;

    if (fileName.isEmpty()){
        mEditor->setReadOnly(true);
        setEditBackgroundColor(QColor("#C7C7C7"));
    }
    else{
        mEditor->setReadOnly(false);
        setEditBackgroundColor(QColor("#FFFFFF"));
    }

    QString shownTitle = "(No Project)";
    QString projName = Utils::enstance()->getCurProjName();
    if (!projName.isEmpty()){
        shownTitle = projName + " Project";
    }

    if (!fileName.isEmpty()){
        shownTitle += " - [ " + fileName + "[*]]";
    }

    mWindow->setWindowTitle(shownTitle);

    mEditor->document()->setModified(false);
    mWindow->setWindowModified(false);
}

void FileManager::setEditBackgroundColor(const QColor &acolor)
{
    // set background color
    mEditor->setAutoFillBackground(true);
    QPalette p = mEditor->palette();
    p.setColor(QPalette::Base, acolor);
    mEditor->setPalette(p);
}