#include <QDebug>
#include "settingswindow.h"
#include "ui_settingswindow.h"

#include "mainwindow.h" // This can't be in settingswindow.h, because mainwindow.h includes settingswindow.h

SettingsWindow::SettingsWindow(MainWindow *main_window, QWidget *parent) :
    main_window_(main_window),
    settings(main_window->settings),
    QDialog(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);
    load_settings();

    //connect(ui->buttonBox,SIGNAL(clicked(QAbstractButton*)),this,SLOT(on_buttonBox_apply()));
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::load_settings()
{
    ui->checkAlwaysHideDetails->setChecked(settings->always_hide_details());
    ui->checkAutoDownload->setChecked(settings->auto_download());
    ui->checkDarkStyle->setChecked(settings->dark_style());
}

void SettingsWindow::save()
{
    settings->setAlways_hide_details(ui->checkAlwaysHideDetails->isChecked());
    settings->setAuto_download(ui->checkAutoDownload->isChecked());
    settings->setDark_style(ui->checkDarkStyle->isChecked());

    main_window_->apply_settings();
}

void SettingsWindow::on_buttonBox_accepted()
{
    save();
}

void SettingsWindow::on_buttonBox_clicked(QAbstractButton *button)
{
    if (button->text()=="Apply") save();
}
