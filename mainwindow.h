#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QTextStream>
#include <QScrollBar>
#include <QFileDialog>
#include <QFileInfo>
#include <QListWidgetItem>
#include "dialog1.h"

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
    void check_download_path();
    void fix_download_path();
    void stop_downloading();
    void open_video();
    void refresh_filelist();
    void refresh_filelist_filtering();
    void select_directory();
    void refresh_interface();
    void download_top_video();
    void add_video_to_download_list();
    void downloading_ended(int a);

    void on_pushButton_clicked();
    void on_pushButton_2_clicked();
    void on_pushButton_3_clicked();
    void on_pushButton_4_clicked();
    void on_lineEdit_2_textChanged();
    void on_listWidget_doubleClicked();
    void on_lineEdit_textChanged();
    void on_comboBox_currentIndexChanged();

private:
    Ui::MainWindow *ui;
    QProcess* youtube_dl;
    ushort download_progress = 0;
    QStringList complete_filelist;
    bool going_to_play_video = false;
};

#endif // MAINWINDOW_H
