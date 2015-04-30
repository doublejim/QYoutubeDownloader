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
#include <QtConcurrent/QtConcurrentRun>
#include <QClipboard>
#include <QMimeData>
#include <QFile>
#include <QTableWidget>

#include "settingswindow.h"
#include "qsettingsinterface.h"
#include "queueitem.h"
#include "dialognewdownload.h"
#include "aboutwindow.h"
#include "osd.h"
#include "filesearcher.h"
#include "mediaitemmap.h"

//using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    QThread NameResolvingThread;

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void add_video_to_download_list(QString url, int format = 0, bool download_subtitles = false, bool download_metadata = false);
    void add_video_to_download_list_DefaultFormat(QString url);
    QSettings settings;
    QSettingsInterface settingsI;

    bool do_not_save_settings = false; // Is set to true if second instance is startet. Avoids messing with window position.
    int default_format();
    void cancel_download();
    void apply_settings_while_running();

public slots:
    void receivedMediaFiles(MediaItemMap itemmap);

signals:
    void sigYouShouldSearchForMedia(QString,QStringList);

private slots:
    void check_download_path();
    void fix_download_path();
    void stop_downloading();
    void delete_selected_item_on_queue();
    void open_video();
    void select_directory();
    void refresh_interface();
    void download_top_video();
    void downloading_ended(int a);
    void create_item_title_from_its_data(QListWidgetItem* item);
    void on_btnStartDownload_clicked();
    void on_btnBrowse_clicked();
    void on_btnAddVideoToQueue_clicked();
    void on_editDownloadPath_textChanged();
    void on_actionSettings_Menu_triggered();
    void on_actionQuit_triggered();
    void on_actionAbout_triggered();
    void on_actionStatusbar_toggled(bool view_statusbar);
    void on_actionOSD_toggled(bool show_osd);
    void customContextMenuRequested(QPoint);
    void toggle_download_format();
    void on_listVideoQueue_doubleClicked();
    void on_btnShowStatus_clicked();
    void on_btnShowOptions_clicked();
    void on_checkAutoDownload_clicked();
    void listVideoQueue_paste();
    void delete_file_from_disk();
    void shortcut_delete();
    void autostart_download(const QModelIndex&, int, int);
    // Media function
    QList<QTableWidgetItem*> make_MediaList_row(MediaItem& mediaitem, int unique_id);
    void fillMediaList(MediaItemMap* itemmap);
    void refresh_MediaList();
    void refresh_MediaList_filtering();
    void on_tableMedia_doubleClicked();
    //
    void on_editSearchTitle_textChanged(const QString &arg1);
    void on_editSearchDate_textChanged(const QString &arg1);
    void on_comboSearchDate_currentIndexChanged(int index);
    void on_editSearchUploader_textChanged(const QString &arg1);

private:
    void init_color_scheme();
    void save_settings();
    void apply_settings_at_startup();
    void play_video(QString file);
    void resolve_title(int item_key, QString url);
    void resolve_playlist_titles(QProcess * youtube_dl);

    // Media search
    QThread fsearchThread;
    FileSearcher* fsearch;
    MediaItemMap allMedia;
    MediaItemMap* currentMedia = nullptr;

    // Main program function
    QProcess* youtube_dl = nullptr;
    QMap <int,QueueItem> queue_items; // item data map.
    int download_progress = 0;
    QStringList complete_filelist;
    int unique_item_key = 0;
    bool going_to_play_video = false;
    QString last_youtubedl_output;
    QString last_audio_destination;

    // Additional
    OSD *osd_;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
