
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(sourceViewer);

    QApplication app(argc, argv);
    MainWindow mainWin;
    mainWin.show();
    return app.exec();
}
