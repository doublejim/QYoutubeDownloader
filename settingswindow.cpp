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
    ui->checkHideStatusButton->setChecked(settings->hide_status_button());
    ui->checkDarkStyle->setChecked(settings->dark_style());
    ui->checkDoNotSaveSizeAndPosition->setChecked(settings->do_not_save_size_and_position());
    ui->editMediaPlayerPath->setText(settings->media_player_path());
    ui->editMediaPlayerArgs->setText(settings->media_player_args());
    ui->editOutputTemplate->setText(settings->output_template());
}

void SettingsWindow::save()
{
    settings->setHide_status_button(ui->checkHideStatusButton->isChecked());
    settings->setDark_style(ui->checkDarkStyle->isChecked());
    settings->setDo_not_save_size_and_position(ui->checkDoNotSaveSizeAndPosition->isChecked());
    settings->setMedia_player_path(ui->editMediaPlayerPath->text());
    settings->setMedia_player_args(ui->editMediaPlayerArgs->text());
    settings->setOutput_template(ui->editOutputTemplate->text());

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

void SettingsWindow::on_btnBrowseMediaPlayer_clicked()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::ExistingFile);
    dialog.setWindowTitle("Select media player executable");
    QDir dir = ui->editMediaPlayerPath->text();
    if (dir.exists())
        dialog.setDirectory(dir.path());
    else
        dialog.setDirectory(".");

    if (dialog.exec())
    {
        ui->editMediaPlayerPath->setText(dialog.selectedFiles().at(0));
    }
}

void SettingsWindow::on_btnSaveSizeAndPosition_clicked()
{
    settings->setSize(main_window_->size());
    settings->setPosition(main_window_->pos());
}

void SettingsWindow::on_btnResetSizeAndPosition_clicked()
{
    settings->setSize(QSize());
    settings->setPosition(QPoint());
}

