#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QLocale>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QString locale = QLocale::system().name();
    QTranslator translator;

    // install translator
    translator.load(QString(":/translations/%1").arg(locale));
    a.installTranslator(&translator);

    // set window icon
    a.setWindowIcon(QIcon(":/images/thindict.svg"));

    // create window
    MainWindow w;

    return a.exec();
}
