#include <QtWebKit>
#include <QWebView>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    // setup ui
    ui->setupUi(this);

    // create web view
    webview = new QWebView(this);
    ui->resultScrollArea->setWidget(webview);

    // create tooltip widget
    toolTipWidget = new ToolTipWidget;

    connect(ui->wordLineEdit, SIGNAL(returnPressed()),
            this, SLOT(slotSearchRequested()));
    connect(ui->searchPushButton, SIGNAL(clicked()),
            this, SLOT(slotSearchRequested()));
    connect(toolTipWidget, SIGNAL(popupResultRequested()),
            this, SLOT(slotPopupResult()));
    connect(QApplication::clipboard(), SIGNAL(selectionChanged()),
            this, SLOT(slotSelectionChanged()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete toolTipWidget;
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

void MainWindow::slotSelectionChanged()
{
    QPoint point = QCursor::pos();

    point.setY(point.y() - 24);
    toolTipWidget->move(point);
    toolTipWidget->show();
}

void MainWindow::slotPopupResult()
{
    ui->wordLineEdit->setText(QApplication::clipboard()->text(QClipboard::Selection));
    slotSearchRequested();
    this->move(QCursor::pos());
    this->show();
    this->activateWindow();
    this->raise();
}

ToolTipWidget::ToolTipWidget(QWidget *parent)
    : QWidget(parent, Qt::ToolTip),
    m_alpha(255)
{
}

void ToolTipWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter;

    (void)e;

    painter.begin(this);
    painter.drawImage(0, 0, QImage(":/images/search.png"));
}

void ToolTipWidget::enterEvent(QEvent *e)
{
    (void)e;

    emit popupResultRequested();
}
