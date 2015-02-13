
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(sourceViewer);

    QApplication app(argc, argv);
    app.setWindowIcon(QIcon(":/images/Source Viewer.png"));
    MainWindow mainWin;
    mainWin.show();
    return app.exec();
}
