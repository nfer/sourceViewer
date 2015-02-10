#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void about();
    void documentWasModified();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void readSettings();
    void writeSettings();
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    bool renameFile(const QString &fileName);
    bool removeFile(const QString &fileName);
    void closeFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    QString strippedName(const QString &fullFileName);
    void getFileInfo(const QString &fileName);

    QPlainTextEdit *textEdit;
    QTextCodec * codec;
    QString curFile;

    QMenu *fileMenu;
    QMenu *editMenu;
    QMenu *encodingMenu;
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
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;

    // encoding menu
    QAction *encodeInAnsiAct;
    QAction *encodeInUTF8NoBom;
    QAction *encodeInUTF8;
    QAction *encodeInUCS2BigEndian;
    QAction *encodeInUCS2BigLittleian;
    QAction *convertToAnsiAct;
    QAction *convertToUTF8NoBom;
    QAction *convertToUTF8;
    QAction *convertToUCS2BigEndian;
    QAction *convertToUCS2BigLittleian;

    // help menu
    QAction *aboutAct;
    QAction *aboutQtAct;
};

#endif
