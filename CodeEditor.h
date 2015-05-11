
#ifndef CODEEDITOR_H
#define CODEEDITOR_H

#include "stable.h"

class LineNumberArea;

class CodeEditor : public QPlainTextEdit
{
    Q_OBJECT

public:
    CodeEditor(QWidget *parent = 0);

    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int  lineNumberAreaWidth();
    bool toggleBookMark(int y);
    int  getLineNumber(int y);
    int  checkBookMark(int line);

static const int BOOKMARKMAXCOUNT = 32;
static const int BOOKMARKMAXWIDTH = 20;

protected:
    void resizeEvent(QResizeEvent *event);

private slots:
    void updateLineNumberAreaWidth(int newBlockCount);
    void highlightCurrentLine();
    void updateLineNumberArea(const QRect &, int);

private:
    QWidget *mLineNumberArea;
    int      mBookMarkList[BOOKMARKMAXCOUNT];

};

class LineNumberArea : public QWidget
{
public:
    LineNumberArea(CodeEditor *editor) : QWidget(editor) {
        mCodeEditor = editor;
    }

    QSize sizeHint() const {
        return QSize(mCodeEditor->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) {
        mCodeEditor->lineNumberAreaPaintEvent(event);
    }

    void mousePressEvent(QMouseEvent * event);

private:
    CodeEditor *mCodeEditor;
};

#endif
