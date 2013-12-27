#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSystemTrayIcon>

namespace Ui {
class MainWindow;
}

class ToolTipWidget;
class QWebView;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void slotSearchRequested();
    void slotSelectionChanged();
    void slotPopupResult();
    void slotEsc();
    void slotSystemTrayActivated(QSystemTrayIcon::ActivationReason reason);
    void slotFocusChanged(QWidget *old, QWidget *now);

private:
    void createSystemTrayIcon();

    Ui::MainWindow *ui;
    QWebView *webview;
    ToolTipWidget *toolTipWidget;
    QSystemTrayIcon *systemTray;
    bool m_popup; // whether this window is a popup or not
};

/**
 * @brief A tooltip widget for user to decide whether to show
 *        result or not.
 */
class ToolTipWidget : public QWidget
{
    Q_OBJECT

public:
    ToolTipWidget(QWidget *parent = 0);
    QSize sizeHint() const {return QSize(24, 24);}
    /**
     * @brief Whether cursor is over the tooltip widget
     */
    bool isOver() const {return m_isOver;}

signals:
    void enterToolTip();
    void leaveToolTip();

protected:
    virtual void paintEvent(QPaintEvent *e);
    virtual void enterEvent(QEvent *e);
    virtual void leaveEvent(QEvent *e);

private:
    int m_alpha;
    bool m_isOver;
};

#endif // MAINWINDOW_H
