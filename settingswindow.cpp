#include <QDebug>
#include "settingswindow.h"
#include "ui_settingswindow.h"

#include "mainwindow.h" // This can't be in settingswindow.h, because mainwindow.h includes settingswindow.h

SettingsWindow::SettingsWindow(MainWindow &main_window, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsWindow),
    main_window(main_window),
    settingsII(&main_window.settings)
{
    ui->setupUi(this);

    settingsII.join("Settings/HideStatusButton", ui->checkHideStatusButton);
    settingsII.join("Settings/DarkStyle", ui->checkDarkStyle);
    settingsII.join("Settings/DoNotSaveSizeAndPosition", ui->checkDoNotSaveSizeAndPosition);
    settingsII.join("Settings/MediaPlayer", ui->editMediaPlayerPath);
    settingsII.join("Settings/MediaPlayerArgs", ui->editMediaPlayerArgs);
    settingsII.join("Settings/OutputTemplate", ui->editOutputTemplate);
    settingsII.join("Settings/Youtube-dlExecutable", ui->editYoutubedlExecutable);
    settingsII.join("Settings/FFMPEGPath", ui->editFFMPEGPath);
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::on_buttonBox_accepted()
{
    settingsII.saveSettingsFromGUI();
    main_window.apply_settings_while_running();
}

void SettingsWindow::on_buttonBox_clicked(QAbstractButton *button)
{
    if (button->text()=="Apply")
    {
        settingsII.saveSettingsFromGUI();
        main_window.apply_settings_while_running();
    }
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
    main_window.settings.setValue("MainWindow/size", main_window.size());
    main_window.settings.setValue("MainWindow/position", main_window.pos());
}

void SettingsWindow::on_btnResetSizeAndPosition_clicked()
{
    main_window.settings.setValue("MainWindow/size", QSize());
    main_window.settings.setValue("MainWindow/position", QPoint());
}

