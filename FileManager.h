
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
        return mEditor;
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

private:
    bool maybeSave();
    void loadFile(const QString &fileName);
    bool saveFile(const QString &fileName);
    bool renameFile(const QString &fileName);
    bool removeFile(const QString &fileName);
    void closeFile(const QString &fileName);
    void setCurrentFile(const QString &fileName);
    void setEditBackgroundColor(const QColor &acolor);

private:
    QMainWindow * mWindow;
    CodeEditor  * mEditor;
    QString       mFileName;
};

#endif
