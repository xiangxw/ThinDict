#include <QSettings>
#include <QFile>
#include <QDir>
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

    settings.setValue("autostart", checked);
    if (checked) {
        QFile::copy(":/thindict.desktop", QDir::homePath() + "/.config/autostart/thindict.desktop");
    } else {
        QDir dir(QDir::homePath() + "/.config/autostart");
        dir.remove("thindict.desktop");
    }
}

/**
 * @brief Load settings
 */
void SettingDialog::loadSettings()
{
    QSettings settings;

    ui->autostartCheckBox->setChecked(settings.value("autostart", false).toBool());
}
