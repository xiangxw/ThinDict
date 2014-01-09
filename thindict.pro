#-------------------------------------------------
#
# Project created by QtCreator 2013-12-18T23:44:30
#
#-------------------------------------------------

QT += core gui webkit
greaterThan(QT_MAJOR_VERSION, 4): QT += webkitwidgets

# libqxt
CONFIG += qxt
QXT += core gui

TARGET = thindict
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    settingdialog.cpp

HEADERS  += mainwindow.h \
    settingdialog.h

FORMS    += \
    ui/mainwindow.ui \
    ui/settingdialog.ui

RESOURCES += \
    rc.qrc

TRANSLATIONS += translations/zh_CN.ts

isEmpty(PREFIX) {
    PREFIX = /opt/$$TARGET
}
target.path = $$PREFIX/bin
desktopfile.files = thindict.desktop
desktopfile.path = /usr/share/applications
logofile.files = images/thindict.svg
logofile.path = /usr/share/pixmaps
INSTALLS += target desktopfile logofile
