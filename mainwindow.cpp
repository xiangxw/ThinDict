#include <QtWebKit>
#include <QWebView>
#include <QApplication>
#include <QDesktopWidget>
#include <QShortcut>
#include <QMenu>
#include <QTimer>
#include <QClipboard>
#include <QMovie>
#include <QToolTip>
#include <QxtGlobalShortcut>
#include <QSettings>
#include <QMessageBox>
#include <QDebug>
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "settingdialog.h"

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

    // create tooltip widget
    toolTipWidget = new ToolTipWidget(this);

    // create setting dialog
    settingDialog = new SettingDialog(this);
    settingDialog->setAttribute(Qt::WA_QuitOnClose, false);

    // create system tray icon
    createSystemTrayIcon();

    // create shortcuts
    createShortcuts();

    // do not quit on close
    this->setAttribute(Qt::WA_QuitOnClose, false);

    connect(ui->wordLineEdit, SIGNAL(returnPressed()),
            this, SLOT(slotSearchRequested()));
    connect(ui->searchPushButton, SIGNAL(clicked()),
            this, SLOT(slotSearchRequested()));
    connect(toolTipWidget, SIGNAL(enterToolTip()),
            this, SLOT(slotStartLoading()));
    connect(toolTipWidget, SIGNAL(enterToolTip()),
            toolTipWidget, SLOT(stopHiding()));
    connect(toolTipWidget, SIGNAL(leaveToolTip()),
            this, SLOT(slotHideToolTipLater()));
    connect(webview, SIGNAL(loadFinished(bool)),
            this, SLOT(slotLoadFinished(bool)));
    connect(settingDialog, SIGNAL(shortcutChanged(QKeySequence)),
            this, SLOT(slotChangeShortcut(QKeySequence)));
    // select a word
    connect(QApplication::clipboard(), SIGNAL(selectionChanged()),
            this, SLOT(slotShowToolTip()));
}

MainWindow::~MainWindow()
{
    delete ui;
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

/**
 * @brief ch is a special char
 */
static bool isSpecial(const QChar &ch)
{
    static const char *chars = "~`!@#$%^&*()-_=+[{]};:'\"\\|,<.>/?* \t\n\r";
    static const int len = strlen(chars);

    for (int i = 0; i < len; ++i) {
        if (ch == chars[i]) {
            return true;
        }
    }
    return false;
}

/**
 * @brief Refine search word
 */
static void refineWord(QString &word)
{
    word = word.trimmed();
    // remove special chars at the beginning of the word
    while (!word.isEmpty() && isSpecial(*(word.begin()))) {
        word.remove(0, 1);
    }
    // remove special chars at the end of the word
    while (!word.isEmpty() && isSpecial(*(word.end() - 1))) {
        word.remove(word.length() - 1, 1);
    }
}

void MainWindow::slotSearchRequested()
{
    QString word;

    word = ui->wordLineEdit->text();
    refineWord(word);

    webview->load(QUrl("http://3g.dict.cn/s.php?q=" + word));
}

void MainWindow::slotShowToolTip()
{
    QPoint point = QCursor::pos();
    static QPixmap pixmap(":/images/tooltip.svg");

    if (QApplication::clipboard()->text(QClipboard::Selection).isEmpty()) {
        return;
    }

    point.setX(point.x() - 12);
    point.setY(point.y() - 36);
    toolTipWidget->move(point);
    toolTipWidget->setPixmap(pixmap);
    toolTipWidget->show();
    slotHideToolTipLater();
}

void MainWindow::slotStartLoading()
{
    static QMovie *movie = new QMovie(":/images/loading.gif", "GIF", this);

    toolTipWidget->setMovie(movie);
    movie->stop();
    movie->start();

    ui->wordLineEdit->setText(QApplication::clipboard()->text(QClipboard::Selection));
    slotSearchRequested();
}

/**
 * @brief Load finished slot
 */
void MainWindow::slotLoadFinished(bool ok)
{
    if (ok) {
        if (!this->isActiveWindow() && resultStillUseful()) { // search word not in thindict window
            ensureAllRegionVisiable();
            if (this->isHidden()) {
                this->show();
            }
            this->activateWindow();
            this->raise();
        }
        m_popup = true;
        toolTipWidget->hide();
        webview->page()->mainFrame()->evaluateJavaScript("scrollTo(0, document.querySelector(\"html body div.content h1\").getClientRects()[0].top)");
    } else {
        m_popup = false;
        if (resultStillUseful()) {
            QToolTip::showText(QCursor::pos(), tr("Search failed! Please check your network."));
        }
        slotHideToolTipLater();
    }
}

/**
 * @brief Select word and set focus
 */
void MainWindow::slotSelectWord()
{
    ui->wordLineEdit->selectAll();
    ui->wordLineEdit->setFocus();
}

/**
 * @brief System tray is activated.
 * @param reason Activate reason.
 */
void MainWindow::slotSystemTrayActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        slotToggleVisible();
    }
}

/**
 * @brief Toggle visible
 */
void MainWindow::slotToggleVisible()
{
    if (this->isActiveWindow()) { // active
        this->hide();
    } else { // not active
        if (!this->isHidden()) { // not hidden
            this->hide(); // for multiple virtual desktop
        }
        moveToScreenCenter();
        this->show();
        this->activateWindow();
        this->raise();
        slotSelectWord();
    }
    m_popup = false;
}

/**
 * @brief Hide tooltip later
 */
void MainWindow::slotHideToolTipLater()
{
    toolTipWidget->hideLater(1500);
}

/**
 * @brief Change shortcut
 */
void MainWindow::slotChangeShortcut(const QKeySequence &key)
{
    toggleVisibleShortcut->disconnect();

    toggleVisibleShortcut->setShortcut(key);
    connect(toggleVisibleShortcut, SIGNAL(activated()),
            this, SLOT(slotToggleVisible()));
}

/**
 * @brief About
 */
void MainWindow::slotAbout()
{
    QMessageBox *messageBox;

    messageBox = new QMessageBox(QMessageBox::NoIcon, tr("About") + " - v0.1.14",
                                 // homepage
                                 QString("<strong>%1</strong>").arg(tr("Homepage"))
                                 + "<p><a href='http://xiangxw.github.io/ThinDict/'>http://xiangxw.github.io/ThinDict/</a></p>"
                                 + "<br>"
                                 // dict source
                                 + QString("<strong>%1</strong>").arg(tr("Dict Source"))
                                 + QString("<p>%1:&nbsp;<a href='http://dict.cn/'>http://dict.cn/</a></p>").arg(tr("Dict.cn")),
                                 QMessageBox::Ok, this);
    messageBox->setAttribute(Qt::WA_DeleteOnClose, true);
    messageBox->setAttribute(Qt::WA_QuitOnClose, false);
    messageBox->setWindowModality(Qt::NonModal);
    messageBox->show();
}

/**
 * @brief Create system tray icon.
 */
void MainWindow::createSystemTrayIcon()
{
    QMenu *menu;

    systemTray = new QSystemTrayIcon(QIcon(":/images/thindict.svg"), this);
    systemTray->setToolTip(tr("ThinDict, a lite dict program"));
    connect(systemTray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(slotSystemTrayActivated(QSystemTrayIcon::ActivationReason)));

    menu = new QMenu(this);
    menu->addAction(QIcon(":/images/settings.svg"), tr("&Settings"),
                    settingDialog, SLOT(show()));
    menu->addAction(QIcon(":/images/about.svg"), tr("&About"),
                    this, SLOT(slotAbout()));
    menu->addAction(QIcon(":/images/quit.svg"), tr("&Quit"), qApp, SLOT(quit()));

    systemTray->setContextMenu(menu);
    systemTray->show();
}

/**
 * @brief Create shortcuts.
 */
void MainWindow::createShortcuts()
{
    QSettings settings;

    // create Esc shortcut
    QShortcut *escShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(escShortcut, SIGNAL(activated()),
            this, SLOT(close()));

    // create select word shortcut
    QShortcut *selectWordShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_L), this);
    connect(selectWordShortcut, SIGNAL(activated()),
            this, SLOT(slotSelectWord()));

    // create quit shortcut
    QShortcut *quitShortcut = new QShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q), this);
    connect(quitShortcut, SIGNAL(activated()),
            qApp, SLOT(quit()));

    // create toggle visible global shortcut
    QKeySequence key(settings.value("ToggleVisibleShortcut").toString());
    toggleVisibleShortcut = new QxtGlobalShortcut(key, this);
    if (!key.isEmpty()) {
        connect(toggleVisibleShortcut, SIGNAL(activated()),
                this, SLOT(slotToggleVisible()));
    }
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

/**
 * @brief Move this window to center of the available screen space.
 */
void MainWindow::moveToScreenCenter()
{
    QPoint pos;
    QPoint center;
    QSize size = this->sizeHint();
    QRect availableRect = qApp->desktop()->availableGeometry();

    center.setX(availableRect.x() + availableRect.width() / 2);
    center.setY(availableRect.y() + availableRect.height() / 2);
    pos.setX(center.x() - size.width() / 2);
    pos.setY(center.y() - size.height() / 2);
    this->move(pos);
}

/**
 * @brief Check whether the search result is still useful.
 */
bool MainWindow::resultStillUseful() const
{
    if (toolTipWidget->isHidden() || !toolTipWidget->underMouse()) {
        return false;
    }
    return true;
}

ToolTipWidget::ToolTipWidget(QWidget *parent)
    : QLabel(parent, Qt::ToolTip)
{
    m_timer = new QTimer(this);

    this->setScaledContents(true);

    // rounded rect
    QPainterPath path;
    QPolygon polygon;
    path.addRoundedRect(0.0, 0.0, this->sizeHint().width(), this->sizeHint().height(),
                        20.0, 20.0, Qt::RelativeSize);
    polygon = path.toFillPolygon().toPolygon();
    this->setMask(QRegion(polygon));
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
