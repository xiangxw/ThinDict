#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include <QLabel>

namespace Ui {
class MainWindow;
}

class ToolTipWidget;
class SettingDialog;
class QxtGlobalShortcut;
class QWebView;
class QTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QSize sizeHint() const {return QSize(400, 300);}

protected:
    virtual bool event(QEvent *event);

private slots:
    void slotSearchRequested();
    void slotShowToolTip();
    void slotStartLoading();
    void slotLoadFinished(bool ok);
    void slotSelectWord();
    void slotSystemTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void slotToggleVisible();
    void slotHideToolTipLater();
    void slotChangeShortcut(const QKeySequence &key);

private:
    void createSystemTrayIcon();
    void createShortcuts();
    void ensureAllRegionVisiable();
    void moveToScreenCenter();

    Ui::MainWindow *ui;
    QWebView *webview;
    ToolTipWidget *toolTipWidget;
    QSystemTrayIcon *systemTray;
    SettingDialog *settingDialog;
    QxtGlobalShortcut *toggleVisibleShortcut;
    bool m_popup; // whether this window is a popup or not
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
