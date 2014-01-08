#-------------------------------------------------
#
# Project created by QtCreator 2013-12-18T23:44:30
#
#-------------------------------------------------

QT += core gui webkit
greaterThan(QT_MAJOR_VERSION, 4): QT += webkitwidgets

TARGET = thindict
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui

RESOURCES += \
    rc.qrc

isEmpty(PREFIX) {
    PREFIX = /opt/$$TARGET
}
target.path = $$PREFIX/bin
desktopfile.files = debian/thindict.desktop
desktopfile.path = /usr/share/applications
logofile.files = images/thindict.svg
logofile.path = /usr/share/pixmaps
INSTALLS += target desktopfile logofile
