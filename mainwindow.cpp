#include <QtWidgets>
#include <QDebug>

#include "config.h"
#include "mainwindow.h"

MainWindow::MainWindow()
{
    textEdit = new QPlainTextEdit;
    setCentralWidget(textEdit);

    createActions();
    createMenus();
    createToolBars();
    createStatusBar();

    readSettings();

    connect(textEdit->document(), SIGNAL(contentsChanged()),
            this, SLOT(documentWasModified()));

    setCurrentFile("");
    setUnifiedTitleAndToolBarOnMac(true);
    codec = NULL;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeSave()) {
        writeSettings();
        event->accept();
    } else {
        event->ignore();
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

    encodeInAnsiAct = new QAction(tr("Encode in ANSI"), this);
    connect(encodeInAnsiAct, SIGNAL(triggered()), this, SLOT(about()));

    encodeInUTF8NoBom = new QAction(tr("Encode in UTF-8 without BOM"), this);
    connect(encodeInUTF8NoBom, SIGNAL(triggered()), this, SLOT(about()));

    encodeInUTF8 = new QAction(tr("Encode in UTF-8"), this);
    connect(encodeInUTF8, SIGNAL(triggered()), this, SLOT(about()));

    encodeInUCS2BigEndian = new QAction(tr("Encode in UCS-2 Big Endian"), this);
    connect(encodeInUCS2BigEndian, SIGNAL(triggered()), this, SLOT(about()));

    encodeInUCS2BigLittleian = new QAction(tr("Encode in UCS-2 Little Endian"), this);
    connect(encodeInUCS2BigLittleian, SIGNAL(triggered()), this, SLOT(about()));

    convertToAnsiAct = new QAction(tr("Convert to ANSI"), this);
    connect(convertToAnsiAct, SIGNAL(triggered()), this, SLOT(about()));

    convertToUTF8NoBom = new QAction(tr("Convert to UTF-8 without BOM"), this);
    connect(convertToUTF8NoBom, SIGNAL(triggered()), this, SLOT(about()));

    convertToUTF8 = new QAction(tr("Convert to UTF-8"), this);
    connect(convertToUTF8, SIGNAL(triggered()), this, SLOT(about()));

    convertToUCS2BigEndian = new QAction(tr("Convert to UCS-2 Big Endian"), this);
    connect(convertToUCS2BigEndian, SIGNAL(triggered()), this, SLOT(about()));

    convertToUCS2BigLittleian = new QAction(tr("Convert to UCS-2 Little Endian"), this);
    connect(convertToUCS2BigLittleian, SIGNAL(triggered()), this, SLOT(about()));

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
    editMenu->addAction(cutAct);
    editMenu->addAction(copyAct);
    editMenu->addAction(pasteAct);

    encodingMenu = menuBar()->addMenu(tr("E&ncoding"));
    encodingMenu->addAction(encodeInAnsiAct);
    encodingMenu->addAction(encodeInUTF8NoBom);
    encodingMenu->addAction(encodeInUTF8);
    encodingMenu->addAction(encodeInUCS2BigEndian);
    encodingMenu->addAction(encodeInUCS2BigLittleian);
    encodingMenu->addSeparator();
    encodingMenu->addAction(convertToUCS2BigLittleian);
    encodingMenu->addAction(convertToUCS2BigEndian);
    encodingMenu->addAction(convertToUTF8);
    encodingMenu->addAction(convertToUTF8NoBom);
    encodingMenu->addAction(convertToAnsiAct);

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::createToolBars()
{
    fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAct);
    fileToolBar->addAction(openAct);
    fileToolBar->addAction(saveAct);

    editToolBar = addToolBar(tr("Edit"));
    editToolBar->addAction(cutAct);
    editToolBar->addAction(copyAct);
    editToolBar->addAction(pasteAct);
}

void MainWindow::createStatusBar()
{
    statusBar()->showMessage(tr("Ready"));
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

void MainWindow::loadFile(const QString &fileName)
{
    //close current file if needed
    if(!curFile.isEmpty())
        closeFile(curFile);

    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    getFileInfo(fileName);

    QTextStream in(&file);
    if(NULL != codec){
        in.setCodec(codec);
    }

#ifndef QT_NO_CURSOR
    QApplication::setOverrideCursor(Qt::WaitCursor);
#endif
    textEdit->setPlainText(in.readAll());
#ifndef QT_NO_CURSOR
    QApplication::restoreOverrideCursor();
#endif

    setCurrentFile(fileName);
    statusBar()->showMessage(tr("File loaded"), 2000);
}

void MainWindow::closeFile(const QString &fileName)
{
    if(fileName.isEmpty())
        return;

    qWarning() << "close file : " << fileName;
    QFile file(fileName);
    file.close();
    codec = NULL;

    textEdit->setPlainText("");

    setCurrentFile("");
    statusBar()->showMessage(tr("File closed"), 2000);
}

bool MainWindow::saveFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, tr(SV_PROGRAM_NAME),
                             tr("Cannot write file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        qWarning() << tr("Cannot write file %1:\n%2.").arg(fileName).arg(file.errorString());
        return false;
    }

    QTextStream out(&file);
    if(NULL != codec){
        out.setCodec(codec);
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
        QMessageBox::warning(this, tr(SV_PROGRAM_NAME),
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
        QMessageBox::warning(this, tr(SV_PROGRAM_NAME),
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
        shownName = "untitled";
        renameAct->setEnabled(false);
        removeAct->setEnabled(false);
    }
    else{
        renameAct->setEnabled(true);
        removeAct->setEnabled(true);
    }
    QString shownTitle = "[*]" + shownName + " - " + SV_PROGRAM_NAME;
    setWindowTitle(shownTitle);
}

QString MainWindow::strippedName(const QString &fullFileName)
{
    return QFileInfo(fullFileName).fileName();
}

void MainWindow::getFileInfo(const QString &fileName)
{
    QProcess p(0);
    p.start("file \""+fileName+"\"");
    p.waitForStarted();
    p.waitForFinished();
    QString fileInfo = QString::fromLocal8Bit(p.readAllStandardOutput());
//    qDebug() << fileInfo;

    QString codecInfo = fileInfo.remove(fileName).remove(": ");
    qDebug() << codecInfo;

    if(codecInfo.contains("UTF-8 Unicode")){
        codec = QTextCodec::codecForName("UTF-8");
    }
}

