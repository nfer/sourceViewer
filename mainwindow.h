#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "stable.h"
#include "windowswatch.h"
#include "fileListFrame.h"

QT_BEGIN_NAMESPACE
class QAction;
class QMenu;
class QPlainTextEdit;
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

protected:
    void closeEvent(QCloseEvent *event);

private slots:
    void newFile();
    void open();
    void close();
    bool rename();
    void remove();
    bool save();
    bool saveAs();
    void undoAll();
    void redoAll();
    void getEncoding();
    void showInEncoding();
    void convertToEncoding();
    void convertToEOL();
    void about();
    void documentWasModified();
    void openSelectFile(const QString & fileName);
    void newProject();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void setupDockWidgets();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void loadFile(const QString &fileName, QTextCodec *codec = NULL, bool hasBOM = false);
    bool saveFile(const QString &fileName);
    bool renameFile(const QString &fileName);
    bool removeFile(const QString &fileName);
    void closeFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    void getFileInfo(const QString &fileName);
    void setEncodingIcon(const QTextCodec *codec, bool hasBOM = false);
    void enableEncodingAcion(bool enabled);

    void saveLayout(QString fileName);
    void loadLayout(QString fileName);
    void setEditBackgroundColor(const QColor &acolor);


    QPlainTextEdit *textEdit;
    QTextCodec * mCodec;
    bool mHasBOM;
    QString curFile;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *eolConvMenu;
    QMenu *encodingMenu;
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
    QAction *convertToWinAct;
    QAction *convertToUnixAct;
    QAction *convertToMacAct;
    QAction *curEOLAct;

    // encoding menu
    QAction *getEncodingAct;
    QAction *encodeInANSIAct;       // ANSI
    QAction *encodeInUTF8WOBAct;    // UTF-8 without BOM
    QAction *encodeInUTF8Act;       // UTF-8
    QAction *encodeInUCS2BEAct;     // UCS-2 Big Endian
    QAction *encodeInUCS2LEAct;     // UCS-2 Little Endian
    QAction *convertToANSIAct;      // ANSI
    QAction *convertToUTF8WOBAct;   // UTF-8 without BOM
    QAction *convertToUTF8Act;      // UTF-8
    QAction *convertToUCS2BEAct;    // UCS-2 Big Endian
    QAction *convertToUCS2LEAct;    // UCS-2 Little Endian
    QAction *curEncodingAct;

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

    FileListFrame * mProjectWindow;
    WindowSwatch  * mSymbolWindow;
    WindowSwatch  * mContextWindows;
    WindowSwatch  * mRelationWindow;
};

#endif //MAINWINDOW_H
