#include <QApplication>
#include <QTranslator>
#include <QLocale>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString locale = QLocale::system().name();
    QTranslator translator;

    // install translator
    translator.load(QString(":/translations/%1").arg(locale));
    a.installTranslator(&translator);

    // set window icon
    a.setWindowIcon(QIcon(":/images/thindict.png"));

    // set application info for QSettings
    a.setOrganizationName("ThinDict");
    a.setApplicationName("thindict");
    a.setApplicationVersion("0.2.2");

    // create window
    MainWindow w;

    return a.exec();
}
