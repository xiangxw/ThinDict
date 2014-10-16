#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QLabel>
#include <QUrl>

namespace Ui {
class MainWindow;
}

class ToolTipWidget;
class SettingDialog;
class QxtGlobalShortcut;
class QWebView;
class QTimer;
namespace Phonon {
    class MediaObject;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QSize sizeHint() const {return QSize(400, 300);}

    enum SearchReason {DefaultSearch, PopupSearch, SelectedSearch};

    Q_INVOKABLE void audioPlay(const QString &src);

protected:
    virtual bool event(QEvent *event);

private slots:
    void slotShowToolTip();
    void slotStartDefaultSearch();
    void slotStartPopupSearch();
    void slotStartSelectedSearch();
    void slotSearchProgress(int progress);
    void slotSelectWord();
    void slotSystemTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void slotToggleVisible();
    void slotToggleVisibleShortcutChanged(const QKeySequence &key);
    void slotSearchSelectedShortcutChanged(const QKeySequence &key);
    void slotPopupSearchToggled(bool toggled);
    void slotAbout();
    void slotTimeout();

private:
    void doSearch(const QString &str);
    bool guessSearch(const QString &str, int count);
    bool removeHyphenGuessSearch(const QString &str);
    void createSystemTrayIcon();
    void createShortcuts();
    void createAudioPlayer();
    void ensureWindowRegionVisible();
    void moveToScreenCenter();
    bool searchResultStillUseful() const;
    bool searchFinishedWithResult() const;
    void notifySearchFailure();
    void scrollToTranslation();

    Ui::MainWindow *ui;
    QWebView *webview;
    ToolTipWidget *toolTipWidget;
    QSystemTrayIcon *systemTray;
    SettingDialog *settingDialog;
    QxtGlobalShortcut *toggleVisibleShortcut;
    QxtGlobalShortcut *searchSelectedShortcut;
    SearchReason m_searchReason;
    Phonon::MediaObject *mediaObject;
    QTimer *m_timer;
};

/**
 * @brief A tooltip widget for user to decide whether to show
 *        result or not.
 */
class ToolTipWidget : public QLabel
{
    Q_OBJECT

public:
    ToolTipWidget(QWidget *parent = 0);
    QSize sizeHint() const {return QSize(24, 24);}
    void hideLater(int msec);

public slots:
    void stopHiding();

signals:
    void enterToolTip();
    void leaveToolTip();

protected:
    virtual void enterEvent(QEvent *e);
    virtual void leaveEvent(QEvent *e);

private:
    QTimer *m_timer;
};

#endif // MAINWINDOW_H
