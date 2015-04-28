
#include "config.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(sourceViewer);

    QApplication app(argc, argv);
    app.setApplicationDisplayName(SV_PROGRAM_NAME);
    app.setWindowIcon(QIcon(":/images/Source Viewer.png"));
    app.setAttribute(Qt::AA_DontShowIconsInMenus);

    // set style sheet
    QFile qss(":/qss/main.qss");
    qss.open(QFile::ReadOnly);
    app.setStyleSheet(qss.readAll());
    qss.close();

    MainWindow mainWin;
    mainWin.show();
    return app.exec();
}
