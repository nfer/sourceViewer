
#include "filelistframe.h"

class FileListDock : public QFrame
{
    Q_OBJECT
public:
    FileListDock(QWidget *parent);
    void setListFile(const QString &fileName);

private:
    virtual QSize sizeHint() const;
    QComboBox            * mFileComboBox;
    QListView            * mFileListView;
    QStandardItemModel   * mFileListModel;

signals:
    void onFileSelected(const QString &);

private slots:
    void currentIndexChanged(int index);
    void currentTextChanged(const QString & text);
    void listViewDoubleClicked(const QModelIndex & index);

protected:
    QSize szHint;
};

FileListDock::FileListDock(QWidget *parent)
    : QFrame(parent)
{
    szHint = QSize(125, 75);
    setMinimumSize(10, 10);

    mFileComboBox = new QComboBox;
    mFileComboBox->setEditable(true);
    QStringList strings;
    strings << "Biao" << "Biao Huang" << "Mac" << "MacBook" << "MacBook Pro" << "Mac Pro";
    mFileComboBox->addItems(strings);
    mFileComboBox->setCurrentText("");
    connect(mFileComboBox, SIGNAL(currentIndexChanged(int)),
            this, SLOT(currentIndexChanged(int)));
    connect(mFileComboBox, SIGNAL(currentTextChanged(const QString & )),
            this, SLOT(currentTextChanged(const QString & )));

    mFileListView = new QListView;
    mFileListModel = new QStandardItemModel;
    mFileListView->setModel(mFileListModel);
    mFileListView->setSelectionBehavior(QAbstractItemView::SelectRows);
    mFileListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    mFileListView->setAlternatingRowColors(true);
    connect(mFileListView, SIGNAL(doubleClicked(const QModelIndex &)),
            this, SLOT(listViewDoubleClicked(const QModelIndex & )));

    QVBoxLayout *mainLayout = new QVBoxLayout;
    mainLayout->addWidget(mFileComboBox);
    mainLayout->addWidget(mFileListView);
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
        mFileListModel->appendRow(item);
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

void FileListDock::listViewDoubleClicked(const QModelIndex & index)
{
    emit onFileSelected(mFileListModel->itemFromIndex(index)->text());
}

FileListFrame::FileListFrame(const QString &dockName, QWidget *parent, Qt::WindowFlags flags)
    : QDockWidget(parent, flags)
{
    setObjectName(dockName + QLatin1String(" Dock Widget"));
    setWindowTitle(dockName);
    setFeatures(features() & ~DockWidgetFloatable);

    mDock = new FileListDock(this);
    mDock->setFrameStyle(QFrame::Box | QFrame::Sunken);
    connect(mDock, SIGNAL(onFileSelected(const QString &)),
            this, SLOT(dockFileSelected(const QString &)));
    setWidget(mDock);
}

void FileListFrame::setListFile(const QString &fileName)
{
    ((FileListDock*)mDock)->setListFile(fileName);
}

void FileListFrame::dockFileSelected(const QString &fileName)
{
    emit onFileSelected(fileName);
}

#include "filelistframe.moc"
