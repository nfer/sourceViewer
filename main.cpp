
#include "config.h"
#include "stable.h"
#include "Utils.h"
#include "mainwindow.h"

void setStyleSheet(QApplication & app, QString fileName)
{
    QFile qss(fileName);
    if (qss.open(QFile::ReadOnly)) {
        qDebug() << "setStyleSheet with file " << fileName;
        app.setStyleSheet(qss.readAll());
        qss.close();
    }
}

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(sourceViewer);

    QApplication app(argc, argv);
    app.setApplicationDisplayName(SV_PROGRAM_NAME);
    app.setWindowIcon(QIcon(":/images/Source Viewer.png"));
    app.setAttribute(Qt::AA_DontShowIconsInMenus);

    setStyleSheet(app, DEFAULT_QSS);
    setStyleSheet(app, Utils::getDocumentPath() +"/" + USER_QSS);

    MainWindow mainWin;
    mainWin.show();
    return app.exec();
}
