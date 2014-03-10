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

/**
 * @brief ch is a special char and should be encoded with percent mark('%')
 * This function is for http://3g.dict.cn, other dict websites may be diffent.
 */
static inline bool isSpecialEncoded(const QChar &ch)
{
    if (ch == '#'
            || ch == '&'
            || ch == '+') {
        return true;
    }
    return false;
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
    if (word.length() == 1) { // for one special char, do not remove it
        return;
    }

    // remove special chars at the beginning of the word
    while (!word.isEmpty() && isSpecial(*(word.begin()))) {
        word.remove(0, 1);
    }
    // remove special chars at the end of the word
    while (!word.isEmpty() && isSpecial(*(word.end() - 1))) {
        word.remove(word.length() - 1, 1);
    }

    if (word.length() == 1 && isSpecialEncoded(word[0])) {
        // encode special search with one special char
        word = QUrl::toPercentEncoding(word);
    }
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent, Qt::Popup),
    ui(new Ui::MainWindow),
    m_searchReason(DefaultSearch)
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
            this, SLOT(slotStartDefaultSearch()));
    connect(ui->searchPushButton, SIGNAL(clicked()),
            this, SLOT(slotStartDefaultSearch()));
    connect(toolTipWidget, SIGNAL(enterToolTip()),
            this, SLOT(slotStartPopupSearch()));
    connect(toolTipWidget, SIGNAL(enterToolTip()),
            toolTipWidget, SLOT(stopHiding()));
    connect(toolTipWidget, SIGNAL(leaveToolTip()),
            this, SLOT(slotHideToolTipLater()));
    connect(webview, SIGNAL(loadFinished(bool)),
            this, SLOT(slotSearchFinished(bool)));
    connect(settingDialog, SIGNAL(toggleVisibleShortcutChanged(QKeySequence)),
            this, SLOT(slotToggleVisibleShortcutChanged(QKeySequence)));
    connect(settingDialog, SIGNAL(searchSelectedShortcutChanged(QKeySequence)),
            this, SLOT(slotSearchSelectedShortcutChanged(QKeySequence)));
}

MainWindow::~MainWindow()
{
    delete ui;
}

bool MainWindow::event(QEvent *event)
{
    if (event->type() == QEvent::WindowDeactivate) {
        switch (m_searchReason) {
        case PopupSearch:
            this->hide();
            toolTipWidget->hide();
            break;

        case SelectedSearch:
            this->hide();
            break;

        default:
            break;
        }
    }
    return QMainWindow::event(event);
}

void MainWindow::doSearch(const QString &str)
{
    QString word = str;

    refineWord(word);

    if (!word.isEmpty()) {
        webview->load(QUrl::fromUserInput("http://3g.dict.cn/s.php?q=" + word));
    }
}

/**
 * @brief Guess the word and do the search
 * @param str Origin search word
 * @param count Guess count(start from 0)
 *
 * @return true: can guess; false: can't guess.
 */
bool MainWindow::guessSearch(const QString &str, int count)
{
    switch (count) {
    case 0:
        return removeHyphenGuessSearch(str);

        // TODO add more guess method here

    default:
        return false;
    }
}

/**
 * @brief Guess search wrod by removing hyphen.
 * @param str Origin search word
 * @return true: can guess; false: can't guess.
 */
inline bool MainWindow::removeHyphenGuessSearch(const QString &str)
{
    QString word = str;
    int hyphen = word.indexOf('-');
    int start;
    int end;
    int tmp;

    if (hyphen >= 0) {
        // find start end
        tmp = hyphen - 1;
        while (tmp >= 0 && word[tmp].isSpace()) {
            --tmp;
        }
        start = tmp + 1;
        // find end index
        tmp = hyphen + 1;
        while (tmp < word.size() && word[tmp].isSpace()) {
            ++tmp;
        }
        end = tmp - 1;
        // remove from start index to end index
        word.remove(start, end - start + 1);
        doSearch(word);
        return true;

    }
    return false;
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

void MainWindow::slotStartDefaultSearch()
{
    m_searchReason = DefaultSearch;
    doSearch(ui->wordLineEdit->text());
}

void MainWindow::slotStartPopupSearch()
{
    static QMovie *movie = new QMovie(":/images/loading.gif", "GIF", this);

    toolTipWidget->setMovie(movie);
    movie->stop();
    movie->start();

    ui->wordLineEdit->setText(QApplication::clipboard()->text(QClipboard::Selection));
    m_searchReason = PopupSearch;
    doSearch(ui->wordLineEdit->text());
}

/**
 * @brief Search finished slot
 */
void MainWindow::slotSearchFinished(bool ok)
{
    static int count = 0; // guess count

    if (!searchResultStillUseful()) {
        return;
    }

    if (ok) { // search succeeded
        if (searchFinishedWithResult()) {
            switch (m_searchReason) {
            case PopupSearch:
                toolTipWidget->hide();
            case SelectedSearch:
                if (!this->isActiveWindow()) {
                    ensureWindowRegionVisible();
                    if (this->isHidden()) {
                        this->show();
                    }
                    this->activateWindow();
                    this->raise();
                }
                break;

            default:
                break;
            }
            slotSelectWord();
            // scroll to content
            webview->page()->mainFrame()->evaluateJavaScript(
                        "scrollTo(0, document.querySelector(\"html body div.content h1\").getClientRects()[0].top)");

            count = 0;
        } else { // search with empty result, guess the search word and try again
            static const int MAX_GUESS_COUNT = 1;
            if (count >= MAX_GUESS_COUNT) { // max guess count reached, stop guessing
                notifySearchFailure();
                count = 0;
            } else { // guess
                while (!guessSearch(ui->wordLineEdit->text(), count)) {
                    if (count >= MAX_GUESS_COUNT) {
                        notifySearchFailure();
                        count = 0;
                        return;
                    }
                    ++count;
                }
            }
        }
    } else { // search failed
        notifySearchFailure();
        count = 0;
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

    m_searchReason = DefaultSearch;
}

/**
 * @brief Search selected word
 */
void MainWindow::slotStartSelectedSearch()
{
    ui->wordLineEdit->setText(QApplication::clipboard()->text(QClipboard::Selection));
    m_searchReason = SelectedSearch;
    doSearch(ui->wordLineEdit->text());
}

/**
 * @brief Hide tooltip later
 */
void MainWindow::slotHideToolTipLater()
{
    toolTipWidget->hideLater(1500);
}

/**
 * @brief Change toggle visible shortcut
 */
void MainWindow::slotToggleVisibleShortcutChanged(const QKeySequence &key)
{
    toggleVisibleShortcut->disconnect();

    if (toggleVisibleShortcut->setShortcut(key)) {
        connect(toggleVisibleShortcut, SIGNAL(activated()),
                this, SLOT(slotToggleVisible()));
    }
}

/**
 * @brief Enable or disable popup search
 */
void MainWindow::slotPopupSearchToggled(bool toggled)
{
    QSettings settings;

    settings.setValue("PopupSearchEnabled", toggled);
    if (toggled) {
        connect(QApplication::clipboard(), SIGNAL(selectionChanged()),
                this, SLOT(slotShowToolTip()));
    } else {
        disconnect(QApplication::clipboard(), SIGNAL(selectionChanged()),
                   this, SLOT(slotShowToolTip()));
    }
}

/**
 * @brief Change search selected shortcut
 */
void MainWindow::slotSearchSelectedShortcutChanged(const QKeySequence &key)
{
    searchSelectedShortcut->disconnect();

    searchSelectedShortcut->setShortcut(key);
    connect(searchSelectedShortcut, SIGNAL(activated()),
            this, SLOT(slotStartSelectedSearch()));
}

/**
 * @brief About
 */
void MainWindow::slotAbout()
{
    QMessageBox *messageBox;

    messageBox = new QMessageBox(QMessageBox::NoIcon, tr("About") + " - v" + qApp->applicationVersion(),
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
    QSettings settings;

    systemTray = new QSystemTrayIcon(QIcon(":/images/thindict.svg"), this);
    systemTray->setToolTip(tr("ThinDict, a lite dict program"));
    connect(systemTray, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            this, SLOT(slotSystemTrayActivated(QSystemTrayIcon::ActivationReason)));

    menu = new QMenu(this);
    // show main window
    menu->addAction(tr("Show &Main Window"),
                    this, SLOT(slotToggleVisible()));
    // settings
    menu->addAction(QIcon(":/images/settings.svg"), tr("&Settings"),
                    settingDialog, SLOT(show()));
    // popup search
    QAction *popupSearchToggleAction;
    bool popupSearchEnabled;
    popupSearchToggleAction = new QAction(tr("&Popup Search"), this);
    popupSearchToggleAction->setCheckable(true);
    popupSearchEnabled = settings.value("PopupSearchEnabled", "true").toBool();
    if (popupSearchEnabled) {
        connect(QApplication::clipboard(), SIGNAL(selectionChanged()),
                this, SLOT(slotShowToolTip()));
    }
    popupSearchToggleAction->setChecked(popupSearchEnabled);
    connect(popupSearchToggleAction, SIGNAL(toggled(bool)),
            this, SLOT(slotPopupSearchToggled(bool)));
    menu->addAction(popupSearchToggleAction);
    // about
    menu->addAction(QIcon(":/images/about.svg"), tr("&About"),
                    this, SLOT(slotAbout()));
    // quit
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
    QKeySequence key;

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
    key = QKeySequence(settings.value("ToggleVisibleShortcut").toString());
    toggleVisibleShortcut = new QxtGlobalShortcut(key, this);
    if (!key.isEmpty()) {
        connect(toggleVisibleShortcut, SIGNAL(activated()),
                this, SLOT(slotToggleVisible()));
    }

    // create search selected global shortcut
    key = QKeySequence(settings.value("SearchSelectedShortcut").toString());
    searchSelectedShortcut = new QxtGlobalShortcut(key, this);
    if (!key.isEmpty()) {
        connect(searchSelectedShortcut, SIGNAL(activated()),
                this, SLOT(slotStartSelectedSearch()));
    }
}

/**
 * @brief Ensure that all region of the window is visible
 */
void MainWindow::ensureWindowRegionVisible()
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
bool MainWindow::searchResultStillUseful() const
{
    bool ret = true;

    switch (m_searchReason) {
    case PopupSearch:
        if (toolTipWidget->isHidden() || !toolTipWidget->underMouse()) {
            ret = false;
        }
        break;

    default:
        break;
    }

    return ret;
}

/**
 * @brief Check whether search finished with result.
 *
 * @return Return false with empty result.
 */
bool MainWindow::searchFinishedWithResult() const
{
    QVariant var;

    var = webview->page()->mainFrame()->evaluateJavaScript(
                "document.querySelector(\"html body div.content h1\").innerHTML");
    return !var.toString().isEmpty();
}

/**
 * @brief Notify search failure
 */
inline void MainWindow::notifySearchFailure()
{
    QToolTip::showText(QCursor::pos(), tr("Search failed! Please check your network."));
    slotHideToolTipLater();
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
