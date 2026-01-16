#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    app.setApplicationName("UEFI Boot Order Manager");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("uefi-settool");

    MainWindow window;
    window.show();

    return app.exec();
}
