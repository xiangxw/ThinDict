#include <QtWebKit>
#include <QWebView>
#include <QApplication>
#include <QDesktopWidget>
#include <QShortcut>
#include <QMenu>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_popup(false)
{
    // setup ui
    ui->setupUi(this);

    // create web view
    webview = new QWebView(this);
    ui->resultScrollArea->setWidget(webview);

    // enable flash for speech
    QWebSettings::globalSettings()->setAttribute(QWebSettings::PluginsEnabled, true);

    // create tooltip widget
    toolTipWidget = new ToolTipWidget;

    // create Esc shortcut
    QShortcut *shortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(shortcut, SIGNAL(activated()),
            this, SLOT(slotEsc()));

    // create system tray icon
    createSystemTrayIcon();

    // do not quit on close
    this->setAttribute(Qt::WA_QuitOnClose, false);

    connect(ui->wordLineEdit, SIGNAL(returnPressed()),
            this, SLOT(slotSearchRequested()));
    connect(ui->searchPushButton, SIGNAL(clicked()),
            this, SLOT(slotSearchRequested()));
    connect(toolTipWidget, SIGNAL(popupResultRequested()),
            this, SLOT(slotPopupResult()));
    // select a word
    connect(QApplication::clipboard(), SIGNAL(selectionChanged()),
            this, SLOT(slotSelectionChanged()));
    // focus changed
    connect(qApp, SIGNAL(focusChanged(QWidget*,QWidget*)),
            this, SLOT(slotFocusChanged(QWidget*,QWidget*)));
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

    if (this->isHidden()) {
        this->move(QCursor::pos());
        this->show();
    }
    if (!this->isActiveWindow()) {
        this->activateWindow();
        this->raise();
    }

    m_popup = true;
}

/**
 * @brief Call this when Esc is pressed.
 */
void MainWindow::slotEsc()
{
    this->hide();
    m_popup = false;
}


/**
 * @brief System tray is activated.
 * @param reason Activate reason.
 */
void MainWindow::slotSystemTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    // show or hide the window when it's not a popup window.
    if (reason == QSystemTrayIcon::Trigger && !m_popup) {
        if (this->isHidden() || !this->isActiveWindow()) {
            this->show();
            this->activateWindow();
            this->raise();
            m_popup = false;
        } else {
            this->hide();
            m_popup = false;
        }
    }
}

/**
 * @brief When the main window is not active.
 * @param old Old widget.
 * @param now New widget.
 */
void MainWindow::slotFocusChanged(QWidget *old, QWidget *now)
{
    if (old && old->window() == this // change focus from main window
            && (!now || now->window() != this) // to another application
            && m_popup) { // it's a popup window
        toolTipWidget->hide();
        this->hide();
        m_popup = false;
    }
}

/**
 * @brief Create system tray icon.
 */
void MainWindow::createSystemTrayIcon()
{
    QMenu *menu;

    systemTray = new QSystemTrayIcon(QIcon(":/images/ldict.png"), this);
    connect(systemTray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(slotSystemTrayActivated(QSystemTrayIcon::ActivationReason)));

    menu = new QMenu(this);
    menu->addAction(tr("&Quit"), qApp, SLOT(quit()));

    systemTray->setContextMenu(menu);
    systemTray->show();
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
    painter.drawImage(0, 0, QImage(":/images/ldict.png"));
}

void ToolTipWidget::enterEvent(QEvent *e)
{
    (void)e;

    emit popupResultRequested();
}
