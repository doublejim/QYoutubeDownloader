#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H
#include <QDialog>
#include <QAbstractButton>

namespace Ui
{
    class SettingsWindow;
}

// These two must be forward declared to be able to create pointers to them
class MainWindow;
class Settings;

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(MainWindow *main_window, QWidget *parent = 0);
    ~SettingsWindow();
    Settings *settings; // I put i here, so it is similar to mainwindow

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_buttonBox_accepted();
    void on_btnBrowseMediaPlayer_clicked();
    void load_settings();
    void save();

private:
    Ui::SettingsWindow *ui;
    MainWindow *main_window_;
};

#endif // SETTINGSWINDOW_H
