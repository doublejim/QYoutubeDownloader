#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include <QDialog>
#include <QAbstractButton>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = 0);
    ~SettingsWindow();
    void load_settings(bool *getsetting_AlwaysHideDetails, bool *getsetting_AutoDownload, bool *getsetting_DarkStyle);

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
    void on_buttonBox_accepted();
    void save();

private:
    Ui::SettingsWindow *ui;

    bool* settingAlwaysHideDetails;
    bool* settingAutoDownload;
    bool* settingDarkStyle;
};

#endif // SETTINGSWINDOW_H
