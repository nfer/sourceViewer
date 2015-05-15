
#include "mainwindow.h"
#include "projectManager.h"
#include "component/Action.h"

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
    newAct = new Action(tr("&New"), tr("Create a new file"),
        QKeySequence::New, QIcon(":/images/new.png"),
        this, SLOT(newFile()), mFileManager);

    openAct = new Action(tr("&Open..."), tr("Open an existing file"),
        QKeySequence::Open, QIcon(":/images/open.png"),
        this, SLOT(open()), mFileManager);

    closeAct = new Action(tr("&Close"), tr("Closes the current file."),
        QKeySequence::listFromString(tr("Ctrl+W")),
        this, SLOT(close()), mFileManager);

    renameAct = new Action(tr("Rename..."), tr("Renames a file on disk and in a project."),
        this, SLOT(rename()), mFileManager);

    removeAct = new Action(tr("&Delete File..."), tr("Deletes a file on disk and in a project."),
        this, SLOT(remove()), mFileManager);

    saveAct = new Action(tr("&Save"), tr("Save the document to disk"),
        QKeySequence::Save, QIcon(":/images/save.png"),
        this, SLOT(save()), mFileManager);

    saveAsAct = new Action(tr("Save &As..."), tr("Save the document under a new name"),
        QKeySequence::listFromString(tr("Ctrl+Shift+S")),
        this, SLOT(saveAs()), mFileManager);

    exitAct = new Action(tr("E&xit"), tr("Exit the application"), QKeySequence::Quit, this);

    undoAct = new Action(tr("&Undo"), tr("Reverses the last editing operation."),
        QKeySequence::Undo, this, SLOT(undo()), mFileManager);

    redoAct = new Action(tr("&Redo"), tr("Recreates the last editing operation after an Undo command."),
        QKeySequence::Redo, this, SLOT(redo()), mFileManager);

    undoAllAct = new Action(tr("U&ndo All"), tr("Undoes all edits to the current file."),
        this, SLOT(undoAll()), mFileManager);

    redoAllAct = new Action(tr("R&edo All"), tr("Redoes all edits to the current file."),
        this, SLOT(redoAll()), mFileManager);

    cutAct = new Action(tr("Cu&t"), tr("Cut the current selection's contents to the clipboard"),
        QKeySequence::Cut, QIcon(":/images/cut.png"),
        this, SLOT(cut()), mFileManager);

    copyAct = new Action(tr("&Copy"), tr("Copy the current selection's contents to the clipboard"),
        QKeySequence::Copy, QIcon(":/images/copy.png"),
        this, SLOT(copy()), mFileManager);

    pasteAct = new Action(tr("&Paste"), tr("Paste the clipboard's contents into the current selection"),
        QKeySequence::Paste, QIcon(":/images/paste.png"),
        this, SLOT(paste()), mFileManager);

    newProjAct = new Action(tr("&New Project..."), tr("Creates a new project."),
        QKeySequence::listFromString(tr("Alt+Shift+N")), this, SLOT(newProject()));

    openProjAct = new Action(tr("&Open Project..."), tr("Opens an existing project."),
        QKeySequence::listFromString(tr("Alt+Shift+P")), this, SLOT(openProject()));

    closeProjAct = new Action(tr("&Close Project"), tr("Closes the current project."),
        QKeySequence::listFromString(tr("Alt+Shift+W")), this, SLOT(closeProject()));

    removeProjAct = new Action(tr("&Remove Project..."), tr("Deletes a project."),
        this, SLOT(removeProject()));

    manageProjFileAct = new Action(tr("&Add and Remove Project Files..."),
        tr("Adds and removes files from current project."), this, SLOT(manageProjFile()));

    syncProjAct = new Action(tr("&Synchronize Files..."), tr("Synchronizes the current project with changes made to files outside the editor."),
        QKeySequence::listFromString(tr("Alt+Shift+S")), this);

    rebuildProjAct = new Action(tr("R&ebuild Project..."), tr("Rebuilds project date files."), this);

    projSettingAct = new Action(tr("Pro&ject Settings..."), tr("Specifies options for Project."), this);

    enableProjectAcion(false);

    aboutAct = new Action(tr("&About"), tr("Show the application's About box"),
        this, SLOT(about()));

    aboutQtAct = new Action(tr("About &Qt"), tr("Show the Qt library's About box"),
        this, SLOT(aboutQt()), qApp);
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
