#ifndef SETTINGDIALOG_H
#define SETTINGDIALOG_H

#include <QDialog>

namespace Ui {
class SettingDialog;
}

class SettingDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingDialog(QWidget *parent = 0);
    ~SettingDialog();

signals:
    void toggleVisibleShortcutChanged(const QKeySequence &key);
    void searchSelectedShortcutChanged(const QKeySequence &key);

private slots:
    void slotToggleAutostart(bool checked);
    void slotToggleVisibleShortcutChanged(const QKeySequence &key);
    void slotSearchSelectedShortcutChanged(const QKeySequence &key);

private:
    void loadSettings();

    Ui::SettingDialog *ui;
};

#endif // SETTINGDIALOG_H
