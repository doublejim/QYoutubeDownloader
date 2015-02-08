#include <QDebug>
#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    //connect(ui->buttonBox,SIGNAL(clicked(QAbstractButton*)),this,SLOT(on_buttonBox_apply()));
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::load_settings(bool* getsetting_AlwaysHideDetails, bool* getsetting_AutoDownload)
{
    ui->checkAlwaysHideDetails->setChecked((*getsetting_AlwaysHideDetails));
    settingAlwaysHideDetails = getsetting_AlwaysHideDetails;

    ui->checkAutoDownload->setChecked((*getsetting_AutoDownload));
    settingAutoDownload = getsetting_AutoDownload;
}

void SettingsWindow::save()
{
    *settingAlwaysHideDetails = ui->checkAlwaysHideDetails->isChecked();
    *settingAutoDownload = ui->checkAutoDownload->isChecked();
}

void SettingsWindow::on_buttonBox_accepted()
{
    save();
}

void SettingsWindow::on_buttonBox_clicked(QAbstractButton *button)
{
    if (button->text()=="Apply") save();
}
