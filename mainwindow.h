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
#include <QShortcut>
#include <QSignalMapper>

#include "settingswindow.h"
#include "settings.h"
#include "queueitem.h"
#include "dialognewdownload.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QApplication *qapp, QWidget *parent = 0);
    ~MainWindow();
    void add_video_to_download_list(QString url, uint format = 0);
    Settings *settings;
    void apply_settings();
    bool do_not_save_settings = false; // Is set to true if second instance is startet. Avoids messing with window position.

private slots:
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
    void downloading_ended(int a);
    void create_item_title_from_its_data(QListWidgetItem* item);
    void video_title_resolved(int item_number);

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
    void on_listVideoQueue_doubleClicked();

private:
    Ui::MainWindow *ui;
    QApplication *qapp_;

    void init_color_scheme();
    void save_settings();
    void restore_settings();
    void start_resolving_video_title(int list_row, QString url);

    QMap <uint,QueueItem> queue_items; // item data map.
    QProcess* youtube_dl;
    ushort download_progress = 0;
    QStringList complete_filelist;
    unsigned int queue_item_counter = 0;
    bool going_to_play_video = false;
};

#endif // MAINWINDOW_H
