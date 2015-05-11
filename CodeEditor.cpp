
#include "CodeEditor.h"

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
    mLineNumberArea = new LineNumberArea(this);

    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));

    updateLineNumberAreaWidth(0);
    highlightCurrentLine();

    for(int i=0; i<BOOKMARKMAXCOUNT; i++)
        mBookMarkList[i] = -1;
}

int CodeEditor::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }

    int space = 3 + fontMetrics().width(QLatin1Char('9')) * digits + BOOKMARKMAXWIDTH;

    return space;
}

bool CodeEditor::toggleBookMark(int y)
{
    int line = getLineNumber(y);
    if (line == -1){
        qWarning() << "getLineNumber failed with pos: " + y;
        return false;
    }

    int slot = checkBookMark(line);
    if (-1 != slot){
        mBookMarkList[slot] = -1;
        qDebug() << "remove bookmark " << line << " in slot " << slot;
        return true;
    }

    for(int i=0; i<BOOKMARKMAXCOUNT; i++){
        if (mBookMarkList[i] == -1){
            mBookMarkList[i] = line;
            qDebug() << "add bookmark " << line << " in slot " << i;
            return true;
        }
    }

    qWarning() << "all slots have bookmarks";
    return false;
}

int CodeEditor::checkBookMark(int line)
{
    for(int i=0; i<BOOKMARKMAXCOUNT; i++){
        if (mBookMarkList[i] == line){
            return i;
        }
    }

    return -1;
}

void CodeEditor::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void CodeEditor::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        mLineNumberArea->scroll(0, dy);
    else
        mLineNumberArea->update(0, rect.y(), mLineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void CodeEditor::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);

    QRect cr = contentsRect();
    mLineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void CodeEditor::highlightCurrentLine()
{
    QList<QTextEdit::ExtraSelection> extraSelections;

    if (!isReadOnly()) {
        QTextEdit::ExtraSelection selection;

        QColor lineColor = QColor(Qt::yellow).lighter(160);

        selection.format.setBackground(lineColor);
        selection.format.setProperty(QTextFormat::FullWidthSelection, true);
        selection.cursor = textCursor();
        selection.cursor.clearSelection();
        extraSelections.append(selection);
    }

    setExtraSelections(extraSelections);
}

void CodeEditor::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(mLineNumberArea);
    painter.fillRect(event->rect(), Qt::lightGray);

    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    int fontHeight = fontMetrics().height();
    int bookMarkWidth = mLineNumberArea->width() - BOOKMARKMAXWIDTH;
    while (block.isValid() && block.isVisible() && top <= event->rect().bottom()) {
        if (bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::black);
            painter.drawText(0, top, bookMarkWidth, fontHeight, Qt::AlignRight, number);

            if (-1 != checkBookMark(blockNumber)){
                painter.setPen(Qt::red);
                painter.drawEllipse(bookMarkWidth + (BOOKMARKMAXWIDTH - fontHeight)/2,
                                    top, fontHeight, fontHeight);
            }
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

int CodeEditor::getLineNumber(int y)
{
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();

    while (block.isValid() && block.isVisible()) {
        if (y >= top && y <= bottom) {
            return blockNumber;
        }

        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }

    return -1;
}

void LineNumberArea::mousePressEvent(QMouseEvent * event)
{
    if (mCodeEditor->toggleBookMark(event->y())){
        repaint();
    }
    else{
        QMessageBox::warning(this, NULL, tr("Already have too many bookmarks."));
    }
}
