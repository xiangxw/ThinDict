#include <QSettings>
#include <QFile>
#include <QDir>
#include <QShortcut>
#include "settingdialog.h"
#include "ui_settingdialog.h"

SettingDialog::SettingDialog(QWidget *parent)
    : QDialog(parent),
      ui(new Ui::SettingDialog)
{
    ui->setupUi(this);

    // load settings
    loadSettings();

    connect(ui->closeButton, SIGNAL(clicked()),
            this, SLOT(close()));
    connect(ui->autostartCheckBox, SIGNAL(toggled(bool)),
            this, SLOT(slotToggleAutostart(bool)));
    connect(ui->toggleVisibleShortcutEdit, SIGNAL(shortcutChanged(QKeySequence)),
            this, SIGNAL(toggleVisibleShortcutChanged(QKeySequence)));
    connect(ui->toggleVisibleShortcutEdit, SIGNAL(shortcutChanged(QKeySequence)),
            this, SLOT(slotToggleVisibleShortcutChanged(QKeySequence)));
    connect(ui->searchSelectedShortcutEdit, SIGNAL(shortcutChanged(QKeySequence)),
            this, SIGNAL(searchSelectedShortcutChanged(QKeySequence)));
    connect(ui->searchSelectedShortcutEdit, SIGNAL(shortcutChanged(QKeySequence)),
            this, SLOT(slotSearchSelectedShortcutChanged(QKeySequence)));


    // Esc
    QShortcut *escShortcut = new QShortcut(QKeySequence(Qt::Key_Escape), this, SLOT(close()));
    (void)escShortcut;
}

SettingDialog::~SettingDialog()
{
    delete ui;
}

/**
 * @brief Toggle autostart
 */
void SettingDialog::slotToggleAutostart(bool checked)
{
    QSettings settings;

    settings.setValue("Autostart", checked);
    if (checked) {
        QFile::copy(":/thindict.desktop", QDir::homePath() + "/.config/autostart/thindict.desktop");
    } else {
        QDir dir(QDir::homePath() + "/.config/autostart");
        dir.remove("thindict.desktop");
    }
}

void SettingDialog::slotToggleVisibleShortcutChanged(const QKeySequence &key)
{
    QSettings settings;

    settings.setValue("ToggleVisibleShortcut", key.toString());
}

void SettingDialog::slotSearchSelectedShortcutChanged(const QKeySequence &key)
{
    QSettings settings;

    settings.setValue("SearchSelectedShortcut", key.toString());
}

/**
 * @brief Load settings
 */
void SettingDialog::loadSettings()
{
    QSettings settings;

    ui->autostartCheckBox->setChecked(
                settings.value("Autostart", false).toBool());
    ui->toggleVisibleShortcutEdit->setKey(
                QKeySequence(settings.value("ToggleVisibleShortcut").toString()));
    ui->searchSelectedShortcutEdit->setKey(
                QKeySequence(settings.value("SearchSelectedShortcut").toString()));
}
