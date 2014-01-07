#-------------------------------------------------
#
# Project created by QtCreator 2013-12-18T23:44:30
#
#-------------------------------------------------

QT += core gui webkit
greaterThan(QT_MAJOR_VERSION, 4): QT += webkitwidgets

TARGET = ldict
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

RESOURCES += \
    rc.qrc

TRANSLATIONS += translations/zh_CN.ts

isEmpty(PREFIX) {
    PREFIX = /opt/$$TARGET
}
target.path = $$PREFIX/bin
desktopfile.files = debian/ldict.desktop
desktopfile.path = /usr/share/applications
logofile.files = images/ldict.svg
logofile.path = /usr/share/pixmaps
INSTALLS += target desktopfile logofile
