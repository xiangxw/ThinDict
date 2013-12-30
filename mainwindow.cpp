#include <QtWebKit>
#include <QWebView>
#include <QApplication>
#include <QDesktopWidget>
#include <QShortcut>
#include <QMenu>
#include <QTimer>
#include <QClipboard>
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
    toolTipWidget = new ToolTipWidget(QImage(":/images/ldict.svg"));

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
    connect(toolTipWidget, SIGNAL(enterToolTip()),
            this, SLOT(slotPopupResult()));
    connect(toolTipWidget, SIGNAL(enterToolTip()),
            toolTipWidget, SLOT(stopHiding()));
    connect(toolTipWidget, SIGNAL(leaveToolTip()),
            this, SLOT(slotHideToolTipLater()));
    // select a word
    connect(QApplication::clipboard(), SIGNAL(selectionChanged()),
            this, SLOT(slotShowToolTip()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete toolTipWidget;
}

bool MainWindow::event(QEvent *event)
{
    // TODO do not hide when the popup window is moving
    if (event->type() == QEvent::WindowDeactivate && m_popup) {
        this->hide();
        toolTipWidget->hide();
        m_popup = false;
    }
    return QMainWindow::event(event);
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

void MainWindow::slotShowToolTip()
{
    QPoint point = QCursor::pos();

    if (QApplication::clipboard()->text(QClipboard::Selection).isEmpty()) {
        return;
    }

    point.setX(point.x() - 12);
    point.setY(point.y() - 36);
    toolTipWidget->move(point);
    toolTipWidget->show();
    slotHideToolTipLater();
}

void MainWindow::slotPopupResult()
{
    ui->wordLineEdit->setText(QApplication::clipboard()->text(QClipboard::Selection));
    slotSearchRequested();

    ensureAllRegionVisiable();
    if (this->isHidden()) {
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
 * @brief Hide tooltip later
 */
void MainWindow::slotHideToolTipLater()
{
    toolTipWidget->hideLater(1500);
}

/**
 * @brief Create system tray icon.
 */
void MainWindow::createSystemTrayIcon()
{
    QMenu *menu;

    systemTray = new QSystemTrayIcon(QIcon(":/images/ldict.svg"), this);
    systemTray->setToolTip(tr("LDict, a lite dict program"));
    connect(systemTray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(slotSystemTrayActivated(QSystemTrayIcon::ActivationReason)));

    menu = new QMenu(this);
    menu->addAction(QIcon(":/images/quit.svg"), tr("&Quit"), qApp, SLOT(quit()));

    systemTray->setContextMenu(menu);
    systemTray->show();
}

/**
 * @brief Ensure that all region of the window is visiable
 */
void MainWindow::ensureAllRegionVisiable()
{
    QRect windowRect(QCursor::pos(), this->sizeHint());
    QRect availableRect = qApp->desktop()->availableGeometry();

    if (windowRect.left() < availableRect.left()) {
        windowRect.moveLeft(availableRect.left());
    }
    if (windowRect.right() > availableRect.right()) {
        windowRect.moveRight(availableRect.right());
    }
    if (windowRect.top() < availableRect.top()) {
        windowRect.moveTop(availableRect.top());
    }
    if (windowRect.bottom() > availableRect.bottom()) {
        windowRect.moveBottom(availableRect.bottom());
    }
    this->setGeometry(windowRect);
}

ToolTipWidget::ToolTipWidget(const QImage &image, QWidget *parent)
    : QWidget(parent, Qt::ToolTip),
      m_image(image)
{
    m_timer = new QTimer(this);
}

/**
 * @brief Hide later
 * @param msec Hide after msec milliseconds.
 */
void ToolTipWidget::hideLater(int sec)
{
    disconnect(m_timer, 0, 0, 0);

    connect(m_timer, SIGNAL(timeout()), this, SLOT(hide()));
    m_timer->start(sec);
}

/**
 * @brief Stop hiding corresponding to hideLater().
 */
void ToolTipWidget::stopHiding()
{
    disconnect(m_timer, 0, 0, 0);
}

void ToolTipWidget::paintEvent(QPaintEvent *e)
{
    QPainter painter;

    (void)e;

    painter.begin(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    // TODO real transparent
    // pseudo transparent
#if QT_VERSION >= 0x050000 // Qt5
    painter.drawPixmap(this->rect(),
                       QApplication::primaryScreen()->grabWindow(0,
                                                                 this->pos().x(),
                                                                 this->pos().y(),
                                                                 this->width(),
                                                                 this->height()));
#else // Qt4
    painter.drawPixmap(this->rect(),
                       QPixmap::grabWindow(QApplication::desktop()->winId(),
                                           this->pos().x(), this->pos().y(),
                                           this->width(), this->height()));
#endif
    // draw icon
    painter.drawImage(this->rect(), m_image);
}

void ToolTipWidget::enterEvent(QEvent *e)
{
    (void)e;

    emit enterToolTip();
}

void ToolTipWidget::leaveEvent(QEvent *e)
{
    (void)e;

    emit leaveToolTip();
}
