#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "stable.h"
#include "Utils.h"
#include "DockWidget.h"
#include "FileManager.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void about();
    void newProject();
    void openProject();
    void closeProject();
    void removeProject();
    void manageProjFile();
    void open();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void setupDockWidgets();

    void enableProjectAcion(bool enabled);
    void openLastProject();

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *projectMenu;
    QMenu *viewMenu;
    QMenu *helpMenu;

    // file menu
    QToolBar *fileToolBar;
    QToolBar *editToolBar;
    QAction *newAct;
    QAction *openAct;
    QAction *closeAct;
    QAction *renameAct;
    QAction *removeAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;

    // edit menu
    QAction *undoAct;
    QAction *redoAct;
    QAction *undoAllAct;
    QAction *redoAllAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;

    // project menu
    QAction *newProjAct;
    QAction *openProjAct;
    QAction *closeProjAct;
    QAction *removeProjAct;
    QAction *manageProjFileAct;
    QAction *syncProjAct;
    QAction *rebuildProjAct;
    QAction *projSettingAct;

    // help menu
    QAction *aboutAct;
    QAction *aboutQtAct;

    ProjectDock   * mProjectWindow;
    DockWidget    * mSymbolWindow;
    DockWidget    * mContextWindows;
    DockWidget    * mRelationWindow;

    FileManager   * mFileManager;
    Utils         * mUtils;
    QString         mProjName;
    QString         mProjStorePath;
};

#endif //MAINWINDOW_H
