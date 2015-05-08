
#include "DockWidget.h"
#include "Utils.h"

class DockFrame : public QFrame
{
    Q_OBJECT
public:
    DockFrame(QWidget *parent);

    virtual QSize sizeHint() const;

protected:
    QSize szHint;
};

DockFrame::DockFrame(QWidget *parent)
    : QFrame(parent)
{
    szHint = QSize(125, 75);
    setMinimumSize(10, 10);

    // set background color
    setAutoFillBackground(true);
    QPalette p = this->palette();
    p.setColor(QPalette::Window, QColor("#D8D8D8"));
    setPalette(p);
}

QSize DockFrame::sizeHint() const
{
    return szHint;
}

DockWidget::DockWidget(const QString &dockName, QWidget *parent, Qt::WindowFlags flags)
    : QDockWidget(parent, flags)
{
    setObjectName(dockName + QLatin1String(" Dock Widget"));
    setWindowTitle(dockName);
    setFeatures(features() & ~DockWidgetFloatable);

    QFrame *swatch = new DockFrame(this);
    swatch->setFrameStyle(QFrame::Box | QFrame::Sunken);
    setWidget(swatch);

    close();
}

class ProjectFrame : public DockFrame
{
    Q_OBJECT
public:
    ProjectFrame(QWidget *parent);
    void updateFileList();

signals:
    void onFileSelected(const QString &);

private slots:
    void currentIndexChanged(int index);
    void currentTextChanged(const QString & text);
    void listViewDoubleClicked(const QModelIndex & index);

private:
    Utils                * mUtils;
    QComboBox            * mFileComboBox;
    QListView            * mFileListView;
    QStandardItemModel   * mFileListModel;
};

ProjectFrame::ProjectFrame(QWidget *parent)
    : DockFrame(parent)
{
    mUtils = Utils::enstance();

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

void ProjectFrame::updateFileList()
{
    mFileListModel->clear();

    QString fileListFile = mUtils->getProjFileListFile();
    if (fileListFile.isEmpty()){
        return ;
    }

    QFile file(fileListFile);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, tr("Application"),
                             tr("Cannot read file %1:\n%2.")
                             .arg(fileListFile)
                             .arg(file.errorString()));
        return;
    }

    QString srcRootPath = mUtils->readString(SRCROOTPATH);

    QTextStream in(&file);
    while(!in.atEnd()){
        QString line=in.readLine();
        if (srcRootPath.length() != 0 && line.startsWith(srcRootPath + "/"))
            line.remove(srcRootPath + "/");
        QStandardItem *item = new QStandardItem(line);
        mFileListModel->appendRow(item);
    }

    mFileListModel->sort(0);
}

void ProjectFrame::currentIndexChanged(int index)
{
    qWarning() << "currentIndexChanged(int index) " << index;
}

void ProjectFrame::currentTextChanged(const QString & text)
{
    qWarning() << "currentTextChanged(const QString & text) " << text;
}

void ProjectFrame::listViewDoubleClicked(const QModelIndex & index)
{
    QString fileName = mFileListModel->itemFromIndex(index)->text();

    if (Utils::isFullFilePath(fileName))
    {
        emit onFileSelected(fileName);
    }
    else{
        QString srcRootPath = Utils::enstance()->readString(SRCROOTPATH);
        emit onFileSelected(srcRootPath + "/" + fileName);
    }
}

ProjectDock::ProjectDock(const QString &dockName, QWidget *parent, Qt::WindowFlags flags)
    : DockWidget(dockName, parent, flags)
{
    ProjectFrame * dock = new ProjectFrame(this);
    dock->setFrameStyle(QFrame::Box | QFrame::Sunken);
    connect(dock, SIGNAL(onFileSelected(const QString &)),
            this, SLOT(projectFileSelected(const QString &)));
    setWidget(dock);
}

void ProjectDock::updateFileList()
{
    ProjectFrame * dock = (ProjectFrame *)widget();
    dock->updateFileList();
}

void ProjectDock::projectFileSelected(const QString &fileName)
{
    emit onFileSelected(fileName);
}

#include "DockWidget.moc"
