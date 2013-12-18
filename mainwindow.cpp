#include <QtWebKit>
#include <QWebView>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    webview = new QWebView(this);
    ui->resultScrollArea->setWidget(webview);

    connect(ui->wordLineEdit, SIGNAL(returnPressed()),
            this, SLOT(slotSearchRequested()));
    connect(ui->searchPushButton, SIGNAL(clicked()),
            this, SLOT(slotSearchRequested()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::slotSearchRequested()
{
    QString word;

    word = ui->wordLineEdit->text();
    word = word.trimmed();
    if (word.isEmpty()) {
        return;
    }

    webview->load(QUrl("http://dict.cn/mini.php?q=" + word));
}
