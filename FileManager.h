
#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include "stable.h"
#include "Utils.h"
#include "Common.h"
#include "CodeEditor.h"

class FileManager : public Object
{
    Q_OBJECT

public:
    FileManager(QMainWindow * window);
    ~FileManager();

    QWidget * getContainer(){
        return mTabWidget;
    }

public slots:
    void newFile();
    void open();
    void close();
    bool rename();
    void remove();
    bool save();
    bool saveAs();
    void undo();
    void redo();
    void undoAll();
    void redoAll();
    void cut();
    void copy();
    void paste();
    void documentWasModified();
    void openSelectFile(const QString & fileName);

private slots:
    void removeSubTab(int index);
    void curTabChanged(int index);

private:
    void showStatusBarMsg(const QString & message, int timeout = 2000);
    CodeEditor * currentEditor(){
        return (CodeEditor *) mTabWidget->currentWidget();
    }
    CodeEditor * createEditor(QString fileName = NULL);
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    bool renameFile(const QString &newFileName);
    bool removeFile();
    void setCurrentFile(const QString &fileName);

private:
    QMainWindow    * mWindow;
    QTabWidget     * mTabWidget;
    int              mNewFileIndex;

    QHash<CodeEditor *, QString> mFileNameHash;
};

#endif
