
#include "filelistframe.h"

class StandardItemListView : public QListView
{
    Q_OBJECT

public:
    void currentChanged(const QModelIndex & current,
                        const QModelIndex & previous);

    void setModel(QStandardItemModel *model);
private:
    QStandardItemModel * mStandardItemModel;
};

void StandardItemListView::setModel(QStandardItemModel * model)
{
    QListView::setModel(model);
    mStandardItemModel = model;
}

void StandardItemListView::currentChanged(const QModelIndex & current,
                            const QModelIndex & previous)
{
    mStandardItemModel->itemFromIndex(current)->setBackground(QBrush(QColor(0,0,255)));

    if(previous.row() == -1)
    {
        // do nothing
    }
    else if(previous.row() % 2 == 1)
    {
        mStandardItemModel->itemFromIndex(previous)->setBackground(QBrush(QColor(242,242,242)));
    }
    else{
        mStandardItemModel->itemFromIndex(previous)->setBackground(QBrush(Qt::white));
    }
}

class FileListDock : public QFrame
{
    Q_OBJECT
public:
    FileListDock(QWidget *parent);
    void setListFile(const QString &fileName);

private:
    virtual QSize sizeHint() const;
    QComboBox * comboBox;
    StandardItemListView * listView;
    QStandardItemModel *standardItemModel;

signals:
    void onFileSelected(const QString &);

private slots:
    void currentIndexChanged(int index);
    void currentTextChanged(const QString & text);
    void doubleClicked(const QModelIndex & index);

protected:
    QSize szHint;
};

FileListDock::FileListDock(QWidget *parent)
    : QFrame(parent)
{
    szHint = QSize(125, 75);
    setMinimumSize(10, 10);

    // set background color
    setAutoFillBackground(true);
    QPalette p = this->palette();
    p.setColor(QPalette::Window, QColor("#D8D8D8"));
    setPalette(p);

    comboBox = new QComboBox;
    comboBox->setEditable(true);
    QStringList strings;
    strings << "Biao" << "Biao Huang" << "Mac" << "MacBook" << "MacBook Pro" << "Mac Pro";
    comboBox->addItems(strings);
    comboBox->setCurrentText("");
    connect(comboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(currentIndexChanged(int)));
    connect(comboBox, SIGNAL(currentTextChanged(const QString & )),
            this, SLOT(currentTextChanged(const QString & )));

    listView = new StandardItemListView;
    standardItemModel = new QStandardItemModel;
    listView->setModel(standardItemModel);
    listView->setSelectionMode(QAbstractItemView::NoSelection);
    listView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    connect(listView, SIGNAL(doubleClicked(const QModelIndex &)),
            this, SLOT(doubleClicked(const QModelIndex & )));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(comboBox);
    mainLayout->addWidget(listView);
    setLayout(mainLayout);
}

void FileListDock::setListFile(const QString &fileName)
{
    QFile file(fileName);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileName)
                             .arg(file.errorString()));
        return;
    }

    QTextStream in(&file);
    QStringList strList;
    while(!in.atEnd()){
        QString line=in.readLine();
        strList.append(line);
    }

    int nCount = strList.size();
    for(int i = 0; i < nCount; i++)
    {
        QString string = static_cast<QString>(strList.at(i));
        QStandardItem *item = new QStandardItem(string);
        if(i % 2 == 1)
        {
            item->setBackground(QBrush(QColor(242,242,242)));
        }
        standardItemModel->appendRow(item);
    }
}

QSize FileListDock::sizeHint() const
{
    return szHint;
}

void FileListDock::currentIndexChanged(int index)
{
    qWarning() << "currentIndexChanged(int index) " << index;
}

void FileListDock::currentTextChanged(const QString & text)
{
    qWarning() << "currentTextChanged(const QString & text) " << text;
}

void FileListDock::doubleClicked(const QModelIndex & index)
{
    emit onFileSelected(standardItemModel->itemFromIndex(index)->text());
}

FileListFrame::FileListFrame(const QString &dockName, QWidget *parent, Qt::WindowFlags flags)
    : QDockWidget(parent, flags)
{
    setObjectName(dockName + QLatin1String(" Dock Widget"));
    setWindowTitle(dockName);

    mDock = new FileListDock(this);
    mDock->setFrameStyle(QFrame::Box | QFrame::Sunken);
    connect(mDock, SIGNAL(onFileSelected(const QString &)),
            this, SLOT(fileSelected(const QString &)));


    setWidget(mDock);

    setFeatures(features() & ~DockWidgetFloatable);
}

void FileListFrame::setListFile(const QString &fileName)
{
    ((FileListDock*)mDock)->setListFile(fileName);
}

void FileListFrame::fileSelected(const QString &fileName)
{
    emit onFileSelected(fileName);
}

#include "filelistframe.moc"
