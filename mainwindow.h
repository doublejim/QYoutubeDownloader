#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QTextStream>
#include <QScrollBar>
#include <QFileDialog>
#include <QFileInfo>
#include <QListWidgetItem>
#include <QDebug>
#include <QMessageBox>
#include <QSettings>
#include <QShortcut>

#include "queueitem.h"
#include "dialognewdownload.h"
#include "settingswindow.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void add_video_to_download_list_from_outside(QString url);

private slots:
    void load_settings();
    void save_settings();

    void check_download_path();
    void fix_download_path();
    void stop_downloading();
    void delete_selected_item_on_queue();
    void open_video();
    void refresh_filelist();
    void refresh_filelist_filtering();
    void select_directory();
    void refresh_interface();
    void download_top_video();
    void add_video_to_download_list();
    void downloading_ended(int a);
    void create_item_title_from_its_data(QListWidgetItem* item);

    void on_btnStartDownload_clicked();
    void on_btnBrowse_clicked();
    void on_btnToggleDetails_clicked();
    void on_btnAddVideoToQueue_clicked();
    void on_editDownloadPath_textChanged();
    void on_listVideos_doubleClicked();
    void on_editSearch_textChanged();
    void on_comboSortType_currentIndexChanged();

    void on_actionSettings_Menu_triggered();
    void customContextMenuRequested(QPoint);
    void toggle_download_format();

private:
    Ui::MainWindow *ui;

    QMap <unsigned int,QueueItem> queue_items;

    QProcess* youtube_dl;
    ushort download_progress = 0;
    QStringList complete_filelist;
    unsigned int queue_item_counter = 0;
    bool going_to_play_video = false;

    bool settingAlwaysHideDetails = false;
    bool settingAutoDownload = false;
};

#endif // MAINWINDOW_H
