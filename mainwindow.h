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
#include <QMutex>
#include <QThread>

#include "settingswindow.h"
#include "settings.h"
#include "queueitem.h"
#include "dialognewdownload.h"
#include "video_title_resolving.h"

using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QThread NameResolvingThread;

public:
    explicit MainWindow(QApplication *qapp, QWidget *parent = 0);
    ~MainWindow();
    void add_video_to_download_list(QString url, uint format = 0);
    Settings *settings;
    void apply_settings();
    bool do_not_save_settings = false; // Is set to true if second instance is startet. Avoids messing with window position.

signals:
    void begin_name_resolving(uint item_key, QString url);

public slots:
    void apply_resolved_video_title(uint item_key, QString title);

private slots:
    //void video_title_resolved(int item_key);
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
    //void resolve_this_video_title(uint item_key, QStringList arguments);
    //void add_video_to_resolve_queue(QListWidgetItem* item);

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

    QMap <uint,QueueItem> queue_items; // item data map.
    QProcess* youtube_dl;
    VideoTitleResolving* resolver;
    //QProcess* youtube_dl_title_resolving;
    //QThread threadTitleResolving;
    //QVector <pair<uint,QStringList> > title_resolving_queue;
    QMutex mutex__io_on_item_list;
    //QMutex* mutex__title_resolving_in_progress;
    //TitleResolvingSignal title_resolving_signal;

    ushort download_progress = 0;
    QStringList complete_filelist;
    unsigned int unique_item_key = 0;
    bool going_to_play_video = false;
};

#endif // MAINWINDOW_H
