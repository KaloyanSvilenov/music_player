#include "mainwindow.h"

#include <QApplication>
#include <QFile>

int main(int argc, char *argv[])
{
    // Optimization
    qputenv("QT_MULTIMEDIA_PREFERRED_PLUGINS", "windowsmediafoundation");
    qputenv("QT_LOGGING_RULES", "qt.multimedia*=false");


    QApplication a(argc, argv);
    MainWindow w;

    QFile file(":/styles/darktheme.qss");
    file.open(QFile::ReadOnly);
    QString styleSheet = QLatin1String(file.readAll());
    a.setStyleSheet(styleSheet);

    w.show();
    return a.exec();
}
