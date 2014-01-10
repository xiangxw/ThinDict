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
    void shortcutChanged(const QKeySequence &key);

private slots:
    void slotToggleAutostart(bool checked);

private:
    void loadSettings();

    Ui::SettingDialog *ui;
};

#endif // SETTINGDIALOG_H
