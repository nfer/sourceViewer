
#include "mainwindow.h"
#include "projectManager.h"

MainWindow::MainWindow() :
    mProjName(),
    mProjStorePath()
{
    mUtils = Utils::enstance();
    mFileManager = new FileManager(this);

    setCentralWidget(mFileManager->getContainer());
    createActions();
    createMenus();
    createToolBars();
    createStatusBar();
    setupDockWidgets();

    mUtils->loadLayout(this);

    openLastProject();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    // FIXME: invoke FileManager close function
    mUtils->saveLayout(this);
    event->accept();
}

void MainWindow::openLastProject()
{
    mProjName = mUtils->getCurProjName();
    if (!mProjName.isEmpty()){
        mProjStorePath = mUtils->getProjStorePath(mProjName);
        mUtils->setCurrentProject(mProjName, mProjStorePath);

        mProjectWindow->updateFileList();
        enableProjectAcion(true);
        openLastOpenedFiles();
    }
}

void MainWindow::openLastOpenedFiles()
{
    //FIXME: invoke FileManager api
}

void MainWindow::snapshotOpenedFiles()
{
    //FIXME: invoke FileManager api
}

void MainWindow::enableProjectAcion(bool enabled)
{
    removeProjAct->setEnabled(enabled);
    manageProjFileAct->setEnabled(enabled);
    syncProjAct->setEnabled(enabled);
    rebuildProjAct->setEnabled(enabled);
}

void MainWindow::about()
{
   QMessageBox::about(this, tr("About Application"),
            tr("The <b>Application</b> example demonstrates how to "
               "write modern GUI applications using Qt, with a menu bar, "
               "toolbars, and a status bar."));
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
}

void MainWindow::closeProject()
{
    if (mProjStorePath.isEmpty() || mProjName.isEmpty())
        return;

    snapshotOpenedFiles();

    setWindowTitle("(No Project)");

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
    connect(newAct, SIGNAL(triggered()), mFileManager, SLOT(newFile()));

    openAct = new QAction(QIcon(":/images/open.png"), tr("&Open..."), this);
    openAct->setShortcuts(QKeySequence::Open);
    openAct->setStatusTip(tr("Open an existing file"));
    connect(openAct, SIGNAL(triggered()), mFileManager, SLOT(open()));

    closeAct = new QAction(tr("&Close"), this);
    closeAct->setShortcuts(QKeySequence::listFromString(tr("Ctrl+W")));
    closeAct->setStatusTip(tr("Closes the current file."));
    connect(closeAct, SIGNAL(triggered()), mFileManager, SLOT(close()));

    renameAct = new QAction(tr("Rename..."), this);
    renameAct->setStatusTip(tr("Renames a file on disk and in a project."));
    connect(renameAct, SIGNAL(triggered()), mFileManager, SLOT(rename()));

    removeAct = new QAction(tr("&Delete File..."), this);
    removeAct->setStatusTip(tr("Deletes a file on disk and in a project."));
    connect(removeAct, SIGNAL(triggered()), mFileManager, SLOT(remove()));

    saveAct = new QAction(QIcon(":/images/save.png"), tr("&Save"), this);
    saveAct->setShortcuts(QKeySequence::Save);
    saveAct->setStatusTip(tr("Save the document to disk"));
    connect(saveAct, SIGNAL(triggered()), mFileManager, SLOT(save()));

    saveAsAct = new QAction(tr("Save &As..."), this);
    saveAsAct->setShortcuts(QKeySequence::listFromString(tr("Ctrl+Shift+S")));
    saveAsAct->setStatusTip(tr("Save the document under a new name"));
    connect(saveAsAct, SIGNAL(triggered()), mFileManager, SLOT(saveAs()));

    exitAct = new QAction(tr("E&xit"), this);
    exitAct->setShortcuts(QKeySequence::Quit);
    exitAct->setStatusTip(tr("Exit the application"));
    connect(exitAct, SIGNAL(triggered()), mFileManager, SLOT(close()));

    undoAct = new QAction(tr("&Undo"), this);
    undoAct->setShortcuts(QKeySequence::Undo);
    undoAct->setStatusTip(tr("Reverses the last editing operation."));
    connect(undoAct, SIGNAL(triggered()), mFileManager, SLOT(undo()));
    undoAct->setEnabled(false);

    redoAct = new QAction(tr("&Redo"), this);
    redoAct->setShortcuts(QKeySequence::Redo);
    redoAct->setStatusTip(tr("Recreates the last editing operation after an Undo command."));
    connect(redoAct, SIGNAL(triggered()), mFileManager, SLOT(redo()));
    redoAct->setEnabled(false);

    undoAllAct = new QAction(tr("U&ndo All"), this);
    undoAllAct->setStatusTip(tr("Undoes all edits to the current file."));
    connect(undoAllAct, SIGNAL(triggered()), mFileManager, SLOT(undoAll()));
    undoAllAct->setEnabled(false);

    redoAllAct = new QAction(tr("R&edo All"), this);
    redoAllAct->setStatusTip(tr("Redoes all edits to the current file."));
    connect(redoAllAct, SIGNAL(triggered()), mFileManager, SLOT(redoAll()));
    redoAllAct->setEnabled(false);

    cutAct = new QAction(QIcon(":/images/cut.png"), tr("Cu&t"), this);
    cutAct->setShortcuts(QKeySequence::Cut);
    cutAct->setStatusTip(tr("Cut the current selection's contents to the clipboard"));
    connect(cutAct, SIGNAL(triggered()), mFileManager, SLOT(cut()));

    copyAct = new QAction(QIcon(":/images/copy.png"), tr("&Copy"), this);
    copyAct->setShortcuts(QKeySequence::Copy);
    copyAct->setStatusTip(tr("Copy the current selection's contents to the "
                             "clipboard"));
    connect(copyAct, SIGNAL(triggered()), mFileManager, SLOT(copy()));

    pasteAct = new QAction(QIcon(":/images/paste.png"), tr("&Paste"), this);
    pasteAct->setShortcuts(QKeySequence::Paste);
    pasteAct->setStatusTip(tr("Paste the clipboard's contents into the current "
                              "selection"));
    connect(pasteAct, SIGNAL(triggered()), mFileManager, SLOT(paste()));

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
    mProjectWindow = new ProjectDock(tr("&Project Window"), this);
    addDockWidget(Qt::RightDockWidgetArea, mProjectWindow);
    viewMenu->addAction(mProjectWindow->toggleViewAction());
    connect(mProjectWindow, SIGNAL(onFileSelected(const QString &)),
            mFileManager, SLOT(openSelectFile(const QString &)));

    mSymbolWindow = new DockWidget(tr("&Symbol Window"), this);
    addDockWidget(Qt::LeftDockWidgetArea, mSymbolWindow);
    viewMenu->addAction(mSymbolWindow->toggleViewAction());

    mContextWindows = new DockWidget(tr("&Context Windows"), this);
    addDockWidget(Qt::BottomDockWidgetArea, mContextWindows);
    viewMenu->addAction(mContextWindows->toggleViewAction());

    mRelationWindow = new DockWidget(tr("&Relation Window"), this);
    addDockWidget(Qt::BottomDockWidgetArea, mRelationWindow);
    viewMenu->addAction(mRelationWindow->toggleViewAction());
}
