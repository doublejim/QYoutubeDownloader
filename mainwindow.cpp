#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "windows.h"
#include <QDebug>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->progressBar->hide();
    ui->progressBar_2->hide();
    ui->label_5->hide();
    ui->label_6->hide();
    ui->lineEdit_2->setText(".");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::open_video() // Requires the path to a media player.
                              // Right now, completely inflexible.
                              // Should be configured via a settings menu instead.
                              // But WORKS if you hard-code the path, etc.
{
    QListWidgetItem *item = ui->listWidget->currentItem();
    QString program = "/usr/bin/mpv";// :/x64/Media Player Classic Home Cinema/mpc-hc64.exe"; // path to media player
    QFile check;
    check.setFileName(program);
    //if (check.exists()==false) return;
    QStringList arguments;
    arguments << '\"'+ui->lineEdit_2->text()+item->text()+'\"'; // path to the video
    QProcess* play_video = new QProcess();
    play_video->start(program,arguments);
    going_to_play_video=false; //(it already did at this point)
}

void MainWindow::select_directory()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setWindowTitle("Select download folder");
    QDir dir = ui->lineEdit_2->text();
    if (dir.exists())
        dialog.setDirectory(ui->lineEdit_2->text());
    else
        dialog.setDirectory(".");

    QStringList filters;
    filters << "*.mp4";
    dialog.setNameFilters(filters);

    if (dialog.exec())
    {
    ui->lineEdit_2->setText(dialog.directory().path());
    }
}

void MainWindow::refresh_interface() // Updates the progress bars and the text output from youtube-dl.
{
    QString newOutput = youtube_dl->readAllStandardOutput(); // nyt output fra youtube-dl

    ui->textEdit->setText(ui->textEdit->toPlainText()+newOutput); // tilføjer teksten til textedit.
    ui->textEdit->verticalScrollBar()->setSliderPosition(ui->textEdit->verticalScrollBar()->maximum()); // sådan at den scroller ned automatisk.

    switch (download_progress)
    {
        case 1: { // begynder at downloade video
                int i=newOutput.lastIndexOf("Destination:");
                if (i!=-1)
                    ++download_progress;
                break;
                }
        case 2: { // downloader video
                int i=newOutput.lastIndexOf("[download]"); // led efter teksten "[download]".
                if (i!=-1)
                {
                    QTextStream stream (&newOutput);
                    stream.seek(i+10); // gå til tallet efter "[download]"-teksten.
                    float value;
                    stream >> value; // gem tallet i value.
                    ui->progressBar->setValue(value); // set progressbaren til den fundne værdi.
                    if (value==100) ++download_progress;
                }
                break;
                }
        case 3: { // begynder at downloade audio
                int i=newOutput.lastIndexOf("Destination:");
                if (i!=-1)
                    ++download_progress;
                break;
                }
        case 4: { // downloader audio
                int i=newOutput.lastIndexOf("[download]");
                if (i!=-1)
                {
                    QTextStream stream (&newOutput);
                    stream.seek(i+10);
                    float value;
                    stream >> value;
                    ui->progressBar_2->setValue(value);
                    if (value==100) ++download_progress;
                }
                break;
                }
    }
}

void MainWindow::check_download_path()
{
    QDir dir = ui->lineEdit_2->text();

    if (dir.exists()==false || ui->lineEdit_2->text()=="")
    {
        complete_filelist.clear(); // must be done: in case the filter is changed, so it doesn't show any files from the previous path.
        ui->listWidget->clear();
        QListWidgetItem *item = new QListWidgetItem("[invalid folder]");
        ui->listWidget->addItem(item);
        ui->listWidget->setStyleSheet("font: italic 8pt \"MS Shell Dlg 2\"");
        return;
    }
    else if (complete_filelist.size()==0)
    {
        ui->listWidget->clear();
        QListWidgetItem *item = new QListWidgetItem("[no mp4 files found...]");
        ui->listWidget->addItem(item);
        ui->listWidget->setStyleSheet("font: italic 8pt \"MS Shell Dlg 2\"");
    }
}

void MainWindow::fix_download_path()
{
    QString dir = QDir::cleanPath(ui->lineEdit_2->text()); // saves a clean version of the path.
    ui->lineEdit_2->setText(dir); // puts back the clean version of the path.

    if (dir[dir.length()-1]!='/' &&
        dir[dir.length()-1]!='\\' )
        ui->lineEdit_2->setText(dir+'/');
}

void MainWindow::download_top_video()
{
    if (ui->listWidget_2->count()==0) return; // quits if the list is empty

    QDir dir = ui->lineEdit_2->text();
    if (dir.exists()==false || ui->lineEdit_2->text()=="") { QMessageBox message; message.setWindowTitle("Invalid folder.");
                               message.setText("The download folder is invalid."); message.exec();
                               return; }
    fix_download_path(); // must happen AFTER the above check, because QDir::cleanPath could cause a crash if the path is invalid.

    QString program = "youtube-dl";
    QStringList arguments;
    QListWidgetItem* item = ui->listWidget_2->item(0);
    arguments << item->text() << "-o" << ui->lineEdit_2->text()+"%(uploader)s - %(title)s.%(ext)s" << "-f" << "bestvideo+bestaudio";

    youtube_dl = new QProcess(this);
    connect (youtube_dl,SIGNAL(readyReadStandardOutput()), this, SLOT(refresh_interface()));
    connect (youtube_dl,SIGNAL(finished(int)),this,SLOT(downloading_ended(int)));

    ui->progressBar->setValue(0);
    ui->progressBar_2->setValue(0);
    ui->progressBar->show();
    ui->progressBar_2->show();
    ui->label_5->show();
    ui->label_6->show();
    ui->pushButton->setText("Pause");
    ++download_progress;
    youtube_dl->start(program,arguments);
}

void MainWindow::stop_downloading()
{
    youtube_dl->close();
    ui->pushButton->setText("Resume");
    download_progress=0;
}

void MainWindow::add_video_to_download_list()
{
    Dialog1 dialog1;
    dialog1.setWindowTitle("Download this video");
    dialog1.setModal(true);
    if (dialog1.exec())
    {
    QString item_name = dialog1.user_input;
    QListWidgetItem *item = new QListWidgetItem(item_name);
    ui->listWidget_2->addItem(item);
    }
}

void MainWindow::add_video_to_download_list_from_outside(QString url)
{
    QListWidgetItem *item = new QListWidgetItem(url);
    ui->listWidget_2->addItem(item);
}

void MainWindow::refresh_filelist() // stores a list of filenames from the download path, in correct order, but doesn't show anything.
{
    QDir dir = ui->lineEdit_2->text();
    if (dir.exists()==false) return;

    QStringList filters;
    filters << "*.mp4";
    ui->listWidget->clear();
    complete_filelist.clear();

    switch (ui->comboBox->currentIndex())
    {
    case 0:
        foreach (QFileInfo file, dir.entryInfoList(filters,QDir::Files,QDir::Time | QDir::Reversed))
            complete_filelist << file.fileName();
        break;
    case 1:
        foreach (QFileInfo file, dir.entryInfoList(filters,QDir::Files,QDir::Name))
            complete_filelist << file.fileName();
        break;
    }
}

void MainWindow::refresh_filelist_filtering() // filters the videos (without searching the harddisk)
{
    ui->listWidget->setStyleSheet("font: 8pt \"MS Shell Dlg 2\"");
    ui->listWidget->clear();
    foreach (QString title,complete_filelist)
    {
        if (title.lastIndexOf(ui->lineEdit->text(),-1,Qt::CaseInsensitive)!=-1)
        {
            QListWidgetItem * item = new QListWidgetItem(title);
            ui->listWidget->addItem(item);
        }
    }
}

void MainWindow::downloading_ended(int a) // delete top video, download next top video. a is not used, but required by the signal.
{
    if (download_progress!=5) return;
    download_progress=0;
    refresh_filelist();
    refresh_filelist_filtering();
    QListWidgetItem *item = ui->listWidget_2->item(0);
    delete item;
    if (ui->listWidget_2->count()>0) { download_top_video(); return; }
    ui->progressBar->hide(); // will only hide progressbars if there are no more videos to download.
    ui->progressBar_2->hide();
    ui->label_5->hide();
    ui->label_6->hide();
    ui->pushButton->setText("Start downloading");
}

void MainWindow::on_pushButton_clicked() // start downloading
{
    if (download_progress==0)
        download_top_video();
    else
        stop_downloading();
}

void MainWindow::on_pushButton_2_clicked() // browse for a directory
{
    select_directory();
}

void MainWindow::on_pushButton_3_clicked() // toggle details
{
    if (ui->textEdit->isHidden()) ui->textEdit->show();
    else ui->textEdit->hide();
}

void MainWindow::on_pushButton_4_clicked() // add video to download list
{
    add_video_to_download_list();
}

void MainWindow::on_lineEdit_2_textChanged() // download folder lineEdit box
{
    if (going_to_play_video==true) return;
    refresh_filelist();
    refresh_filelist_filtering();
    check_download_path();
}

void MainWindow::on_listWidget_doubleClicked() // downloaded videos list doubleclick
{
    going_to_play_video=true;
    check_download_path();
    fix_download_path();
    open_video();
}

void MainWindow::on_lineEdit_textChanged() // search text changed
{
    refresh_filelist_filtering();
    if (ui->lineEdit->text().length()==0) check_download_path();
}

void MainWindow::on_comboBox_currentIndexChanged() // new type of sorting
{
    refresh_filelist();
    refresh_filelist_filtering();
    check_download_path();
}
