
#include "windowswatch.h"

class WindowDock : public QFrame
{
    Q_OBJECT
public:
    WindowDock(QWidget *parent);

    virtual QSize sizeHint() const;

protected:
    QSize szHint;
};

WindowDock::WindowDock(QWidget *parent)
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

QSize WindowDock::sizeHint() const
{
    return szHint;
}

WindowSwatch::WindowSwatch(const QString &dockName, QWidget *parent, Qt::WindowFlags flags)
    : QDockWidget(parent, flags)
{
    setObjectName(dockName + QLatin1String(" Dock Widget"));
    setWindowTitle(dockName);

    QFrame *swatch = new WindowDock(this);
    swatch->setFrameStyle(QFrame::Box | QFrame::Sunken);

    setWidget(swatch);

    setFeatures(features() & ~DockWidgetFloatable);
}

#include "windowswatch.moc"
