#include "mainwindow.h"
#include "ui_mainwindow.h"

//using namespace std;

MainWindow::MainWindow(QApplication *qapp, QWidget *parent) :
    qapp_(qapp),
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->progressVideo->hide();
    ui->progressAudio->hide();
    ui->labelVideo->hide();
    ui->labelAudio->hide();

    settings = new Settings(qApp->applicationDirPath());
    restore_settings();

    if (ui->editDownloadPath->text()=="")  ui->editDownloadPath->setText(".");

    QShortcut* shortcut_del = new QShortcut(QKeySequence(Qt::Key_Delete),this);
    connect(shortcut_del, SIGNAL(activated()), this, SLOT(shortcut_delete()));

    QShortcut* shortcut_enter = new QShortcut(QKeySequence(Qt::Key_Return), ui->listVideos);
    connect(shortcut_enter, SIGNAL(activated()), this, SLOT(on_listVideos_doubleClicked()));

    QShortcut* shortcut_ctrl_v = new QShortcut(QKeySequence(tr("Ctrl+V")), ui->listVideoQueue);
    connect(shortcut_ctrl_v, SIGNAL(activated()),this,SLOT(listVideoQueue_paste()));

    connect(ui->listVideoQueue->model(), SIGNAL(rowsInserted(const QModelIndex&, int, int)), this, SLOT(autostart_download(const QModelIndex&, int, int)));

    ui->listVideoQueue->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listVideoQueue, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenuRequested(QPoint)));
    qRegisterMetaType<QVector<int> >("QVector<int>");
}

MainWindow::~MainWindow()
{
    if(!do_not_save_settings)
    {
        save_settings();
        delete settings; //sync() settings_
    }
    delete ui;
}

//Settings that should be applied on program start
void MainWindow::restore_settings()
{
    if(settings->size() != QSize()) // size not set, let wm chose
        resize(settings->size());
    if(settings->position() != QPoint()) // pos not set, let wm chose
        move(settings->position());
    ui->editDownloadPath->setText(settings->download_path());
    ui->editSearch->setText(settings->last_search());
    ui->comboSortType->setCurrentIndex(settings->combo_sort_type());
    ui->checkOpenInPlayerAfterDownload->setChecked(settings->open_in_player_after_download());
    ui->checkAutoDownload->setChecked(settings->auto_download());
    ui->stackedQueueInfoOptions->setCurrentIndex(settings->stacked_widget_active_page());
    ui->checkExitWhenFinshed->setChecked(settings->exit_when_finshed());

    ui->actionStatusbar->setChecked(settings->show_statusbar());
    if(settings->show_statusbar())
        ui->statusBar->show();
    else
        ui->statusBar->hide();

    if (settings->expand_status_and_settings())
        ui->stackedQueueInfoOptions->show();
    else
        ui->stackedQueueInfoOptions->hide();

    // it doesn't yet load the download queue.
    apply_settings();
}

// Settings that should be applied while program is running
// This function is also called from the settings window, whenever ok or apply is pressed
void MainWindow::apply_settings()
{
    if (settings->hide_status_button())
    {
        ui->btnShowStatus->hide();
        if (ui->stackedQueueInfoOptions->currentIndex()==0)
            ui->stackedQueueInfoOptions->setCurrentIndex(1);
    }
    else
    {
        ui->btnShowStatus->show();
    }

    init_color_scheme();
}

int MainWindow::default_format()
{
    if(ui->radioAudioVideo->isChecked())
        return 0;
    else
        return 1;
}

void MainWindow::save_settings()
{
    if(!settings->do_not_save_size_and_position())
    {
        settings->setSize(size());
        settings->setPosition(pos());
    }

    settings->setOpen_in_player_after_download(ui->checkOpenInPlayerAfterDownload->isChecked());
    QDir dir (ui->editDownloadPath->text());

    if (dir.exists())
        settings->setDownload_path(dir.path());

    settings->setExpand_status_and_settings(!ui->stackedQueueInfoOptions->isHidden());
    settings->setLast_search(ui->editSearch->text());
    settings->setCombo_sort_type(ui->comboSortType->currentIndex());
    settings->setAuto_download(ui->checkAutoDownload->isChecked());
    settings->setStacked_widget_active_page(ui->stackedQueueInfoOptions->currentIndex());
    settings->setExit_when_finshed(ui->checkExitWhenFinshed->isChecked());

    // it doesn't yet save the download queue.
}


void MainWindow::init_color_scheme()
{
    if(settings->dark_style())
    {
        QFile f(":qdarkstyle/style.qss");
        if (!f.exists())
        {
            printf("Unable to set stylesheet, file not found\n");
        }
        else
        {
            f.open(QFile::ReadOnly | QFile::Text);
            QTextStream ts(&f);
            qapp_->setStyleSheet(ts.readAll());
        }
    }
    else
    {
        qapp_->setStyleSheet("fusion");
    }
}

void MainWindow::listVideoQueue_paste()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

    QString text;

    if (mimeData->hasText())
    {
        text = mimeData->text();
        add_video_to_download_list(text);
    }
}

// TODO: Make it possible to send arguments to player.
void MainWindow::open_video()
{
    fix_download_path();

    QListWidgetItem *item = ui->listVideos->currentItem();
    QString dir = ui->editDownloadPath->text();

    QString file = dir + item->text();

    play_video(file);

    going_to_play_video = false; //(it already did at this point)
}

void MainWindow::play_video(QString file)
{
    QString program = settings->media_player_path();
    QStringList arguments;

    if (settings->media_player_args() != "")
        arguments << settings->media_player_args() << file;
    else
        arguments << file;

    QProcess* play_video = new QProcess();
    play_video->start(program, arguments);
}

void MainWindow::select_directory()
{
    QFileDialog dialog;
    dialog.setFileMode(QFileDialog::Directory);
    dialog.setWindowTitle("Select download folder");
    QDir dir = ui->editDownloadPath->text();
    if (dir.exists())
        dialog.setDirectory(ui->editDownloadPath->text());
    else
        dialog.setDirectory(QDir::currentPath() + "/");

    if (dialog.exec())
    {
        ui->editDownloadPath->setText(dialog.directory().path());
    }
}

void MainWindow::refresh_interface() // Updates the progress bars and the text output from youtube-dl.
{
    QString newOutput = youtube_dl->readAllStandardOutput(); // nyt output fra youtube-dl

    ui->textDetails->setText(ui->textDetails->toPlainText()+newOutput); // tilføjer teksten til textedit.
    ui->textDetails->verticalScrollBar()->setSliderPosition(ui->textDetails->verticalScrollBar()->maximum()); // sådan at den scroller ned automatisk.

    switch (download_progress)
    {
        case 1: { // begynder at downloade video
                ui->statusBar->showMessage("Initialising video download...");
                int i=newOutput.lastIndexOf("Destination:");
                if (i!=-1)
                {
                    int current_item_key = ui->listVideoQueue->item(0)->data(Qt::UserRole).toInt();
                    if (queue_items[current_item_key].format==0)  // Download Audio + video
                        download_progress = 2;
                    else // Download Audio
                    {
                        download_progress = 4;
                        last_audio_destination = newOutput.right(newOutput.length() - i - 13); // Not so nice, but works
                        last_audio_destination = last_audio_destination.simplified(); // remove trailing newline
                    }
                }
                break;
                }
        case 2: { // downloader video
                ui->statusBar->showMessage("Downloading video: filename - ? MiB");
                int i=newOutput.lastIndexOf("[download]"); // led efter teksten "[download]".
                if (i!=-1)
                {
                    QTextStream stream (&newOutput);
                    stream.seek(i+10); // gå til tallet efter "[download]"-teksten.
                    float value;
                    stream >> value; // gem tallet i value.
                    ui->progressVideo->setValue(value); // set progressbaren til den fundne værdi.
                    if (value==100) ++download_progress;
                }
                break;
                }
        case 3: { // begynder at downloade audio
                ui->statusBar->showMessage("Initialising audio download...");
                int i=newOutput.lastIndexOf("Destination:");
                if (i!=-1)
                    ++download_progress;
                break;
                }
        case 4: { // downloader audio
                ui->statusBar->showMessage("Downloading audio: filename - ? MiB");
                int i=newOutput.lastIndexOf("[download]");
                if (i!=-1)
                {
                    QTextStream stream (&newOutput);
                    stream.seek(i+10);
                    float value;
                    stream >> value;
                    ui->progressAudio->setValue(value);
                    if (value==100) ++download_progress;
                }
                break;
                }
    }
    last_youtubedl_output = newOutput;
}

void MainWindow::check_download_path()
{
    QDir dir = ui->editDownloadPath->text();

    if (dir.exists()==false || ui->editDownloadPath->text() == "")
    {
        complete_filelist.clear(); // must be done: in case the filter is changed, so it doesn't show any files from the previous path.
        ui->listVideos->clear();
        QListWidgetItem *item = new QListWidgetItem("[invalid folder]");
        ui->listVideos->addItem(item);
        ui->listVideos->setStyleSheet("font: italic 8pt \"MS Shell Dlg 2\"");
        return;
    }
    else if (complete_filelist.size() == 0)
    {
        ui->listVideos->clear();
        QListWidgetItem *item = new QListWidgetItem("[no mp4 files found...]");
        ui->listVideos->addItem(item);
        ui->listVideos->setStyleSheet("font: italic 8pt \"MS Shell Dlg 2\"");
    }
}

void MainWindow::fix_download_path()
{
    QString dir = QDir::cleanPath(ui->editDownloadPath->text()); // saves a clean version of the path.
    if(dir == ".")
        dir = QDir::currentPath();

    ui->editDownloadPath->setText(dir + '/');
}

void MainWindow::download_top_video()
{
    if (ui->listVideoQueue->count()==0) return; // quits if the list is empty

    QDir dir = ui->editDownloadPath->text();
    if (dir.exists()==false || ui->editDownloadPath->text() == "") { QMessageBox message; message.setWindowTitle("Invalid folder.");
                               message.setText("The download folder is invalid."); message.exec();
                               return; }
    fix_download_path(); // must happen AFTER the above check, because QDir::cleanPath causes a crash if the path is empty.

    QString program = settings->youtube_dl_executable();
    QStringList arguments;

    QListWidgetItem* current_item = ui->listVideoQueue->item(0);
    int current_item_key = current_item->data(Qt::UserRole).toInt();
    ushort format = queue_items[current_item_key].format;

    QString format_to_download;
    switch (format)
    {
        case 1: format_to_download = "bestaudio"; break;
        default: format_to_download = "bestvideo+bestaudio"; break;
    }

    arguments << "-o" << ui->editDownloadPath->text() + settings->output_template()
              << "-f" << format_to_download
              << "--ffmpeg-location" << settings->ffmpeg_path()
              << "--merge-output-format" << "mkv"
              << queue_items[current_item_key].url;

    youtube_dl = new QProcess(this);
    connect (youtube_dl, SIGNAL(readyReadStandardOutput()), this, SLOT(refresh_interface()));
    connect (youtube_dl, SIGNAL(finished(int)), this, SLOT(downloading_ended(int)));

    ui->progressVideo->setValue(0);
    ui->progressAudio->setValue(0);
    if(format == 0)
    {
        ui->progressVideo->show();
        ui->labelVideo->show();
    }
    ui->progressAudio->show();
    ui->labelAudio->show();
    ui->btnStartDownload->setText("Pause");
    ++download_progress;
    youtube_dl->start(program,arguments);
}

void MainWindow::on_listVideoQueue_doubleClicked() // edit item
{
    DialogNewDownload dialog1;
    dialog1.setWindowTitle("Download this video");
    dialog1.setModal(true);
    QListWidgetItem *item = ui->listVideoQueue->item( ui->listVideoQueue->currentRow() );
    int current_item_key = item->data(Qt::UserRole).toInt();

    dialog1.load(queue_items[current_item_key].title, queue_items[current_item_key].format);
    // the solution to loading the settings should also be applied here!

    if (dialog1.exec())
    {
        QString url = dialog1.user_input;
        int format = dialog1.format_to_download;
        queue_items[current_item_key].setTitle(url);
        queue_items[current_item_key].setFormat(format);
        create_item_title_from_its_data(item);
        ui->listVideoQueue->addItem(item);
    }

    if (settings->auto_download() && download_progress==0) download_top_video();
}

void MainWindow::delete_selected_item_on_queue()
{
    if(ui->listVideoQueue->selectedItems().contains(ui->listVideoQueue->item(0)))
    {
        if(youtube_dl != NULL)
            youtube_dl->close();
        download_progress=0;
        ui->btnStartDownload->setText("Start downloading");
        ui->progressVideo->hide();
        ui->progressAudio->hide();
        ui->labelVideo->hide();
        ui->labelAudio->hide();
    }

    foreach(QListWidgetItem *item, ui->listVideoQueue->selectedItems())
    {
        int item_key = item->data(Qt::UserRole).toInt();
        delete item;
        queue_items.remove(item_key);
    }
}

void MainWindow::stop_downloading()
{
    youtube_dl->close();
    ui->btnStartDownload->setText("Resume");
    download_progress=0;
}

void MainWindow::create_item_title_from_its_data(QListWidgetItem* item)
{
    int current_item_key = item->data(Qt::UserRole).toInt();

    switch (queue_items[current_item_key].format)
    {
        case 0: item->setData(Qt::DisplayRole,queue_items[current_item_key].title+" | Video+Audio"); break;
        case 1: item->setData(Qt::DisplayRole,queue_items[current_item_key].title+" | Audio only"); break;
    }
}

void MainWindow::resolve_title(int item_key, QString url)
{
    try
    {
        QString program = settings->youtube_dl_executable();

        QStringList arguments;
        arguments << "-e" << "--get-id" << url;

        QProcess get_title;

        get_title.start(program, arguments);
        get_title.waitForReadyRead();

        if(url.contains("youtube.com/playlist") || url.contains("youtube.com/user") || url.contains("youtube.com/channel"))
        {
            resolve_playlist_titles(&get_title);
            get_title.close();
            return;
        }

        QString title = get_title.readAllStandardOutput().simplified();

        get_title.close();

        if (title == "")
            return;

        QListWidgetItem *item;
        for(int i = 0; i < ui->listVideoQueue->count(); i++)
        {
            item = ui->listVideoQueue->item(i);
            if (item->data(Qt::UserRole).toInt() == item_key)
            {
                queue_items[item_key].title = title;
                create_item_title_from_its_data(item);
                ui->listVideoQueue->update();
                return;
            }
        }
    }
    catch(int e)
    {}
}

void MainWindow::resolve_playlist_titles(QProcess *get_title)
{
    QTextStream stream(get_title);
    bool is_title = true;
    QString line = stream.readLine();
    QListWidgetItem *item;

    while (!line.isNull())
    {
        //For each video it gets 2 lines, 1st Title, 2nd video ID
        if(is_title)
        {
            item = new QListWidgetItem(line);
            item->setData(Qt::UserRole, unique_item_key); // højeste nummer bliver til key
            queue_items[unique_item_key].title = line;
            queue_items[unique_item_key].format = ui->radioAudioVideo->isChecked() ? 0 : 1;
            create_item_title_from_its_data(item);
        }
        else
        {
            queue_items[unique_item_key].url = "https://www.youtube.com/watch?v=" + line;
            ui->listVideoQueue->addItem(item);
            ++unique_item_key;
        }

        if(get_title->isOpen())
            get_title->waitForReadyRead();

        is_title = !is_title;
        line = stream.readLine();
    }
}

void MainWindow::autostart_download(const QModelIndex&, int, int)
{
    if (ui->listVideoQueue->count() == 1 && settings->auto_download() && download_progress==0)
        download_top_video();
}

void MainWindow::add_video_to_download_list(QString url, int format)
{
    QListWidgetItem *item = new QListWidgetItem(url);

    if(!url.contains("youtube.com/playlist") && !url.contains("youtube.com/user") && !url.contains("youtube.com/channel"))
    {
        item->setData(Qt::UserRole, unique_item_key); // højeste nummer bliver til key

        queue_items[unique_item_key].url = url;
        queue_items[unique_item_key].title = url;
        queue_items[unique_item_key].format = format;

        create_item_title_from_its_data(item);
        ui->listVideoQueue->addItem(item);
    }

    QtConcurrent::run(this, &MainWindow::resolve_title, unique_item_key, url);

    ++unique_item_key;
}

void MainWindow::refresh_filelist() // stores a list of filenames from the download path, in correct order, but doesn't show anything.
{
    QDir dir = ui->editDownloadPath->text();
    if (dir.exists()==false) return;

    QStringList filters;
    filters << "*.mp4" << "*.m4a" << "*.mp3" << "*.ogg" << "*.flv" << "*.webm" << "*.mkv";
    ui->listVideos->clear();
    complete_filelist.clear();

    switch (ui->comboSortType->currentIndex())
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
    ui->listVideos->setStyleSheet("font: 8pt \"MS Shell Dlg 2\"");
    ui->listVideos->clear();
    foreach (QString title,complete_filelist)
    {
        if (title.lastIndexOf(ui->editSearch->text(),-1,Qt::CaseInsensitive)!=-1)
        {
            QListWidgetItem * item = new QListWidgetItem(title);
            ui->listVideos->addItem(item);
        }
    }
}

void MainWindow::downloading_ended(int a) // delete top video, download next top video. a is not used, but required by the signal.
{
    if (download_progress!=5) return;
    ui->statusBar->showMessage("Downloading finished.");
    if(ui->checkOpenInPlayerAfterDownload->isChecked())
    {
        int item_key = ui->listVideoQueue->item(0)->data(Qt::UserRole).toInt();
        if (queue_items[item_key].format == 0 )
        {
            qDebug() << last_youtubedl_output;
            QRegExp rx("\"([^\"]*)\""); // Match string in quotes
            rx.indexIn(last_youtubedl_output); // Search in last_output
            if(rx.cap(1) != "") // If file found, play it
                play_video(rx.cap(1));
        }
        else
        {
            if(last_audio_destination != "")
                play_video(last_audio_destination);
        }
    }
    download_progress=0;
    refresh_filelist();
    refresh_filelist_filtering();
    QListWidgetItem *item = ui->listVideoQueue->item(0);
    int item_key = item->data(Qt::UserRole).toInt();
    delete item;
    queue_items.remove(item_key);
    if (ui->listVideoQueue->count()>0) { download_top_video(); return; }
    ui->progressVideo->hide(); // will only hide progressbars if there are no more videos to download.
    ui->progressAudio->hide();
    ui->labelVideo->hide();
    ui->labelAudio->hide();
    ui->btnStartDownload->setText("Start downloading");
    if(ui->checkExitWhenFinshed->isChecked() && ui->listVideoQueue->count() == 0)
        qApp->quit();
}

void MainWindow::on_btnStartDownload_clicked() // start downloading
{
    if (ui->listVideoQueue->count()==0) return;
    if (download_progress==0)
        download_top_video();
    else
        stop_downloading();
}

void MainWindow::on_btnBrowse_clicked() // browse for a directory
{
    select_directory();
}

void MainWindow::on_btnAddVideoToQueue_clicked() // add video to download list
{
    DialogNewDownload dialog1;
    dialog1.setWindowTitle("Download this video");
    dialog1.setModal(true);
    if (dialog1.exec())
    {
        QString url = dialog1.user_input;
        int format = dialog1.format_to_download;
        add_video_to_download_list(url, format);
    }
}

void MainWindow::on_editDownloadPath_textChanged() // download folder lineEdit box
{
    if (going_to_play_video==true) return;
    refresh_filelist();
    refresh_filelist_filtering();
    check_download_path();
}

void MainWindow::on_listVideos_doubleClicked() // downloaded videos list doubleclick
{
    going_to_play_video=true;
    check_download_path();
    fix_download_path();
    open_video();
}

void MainWindow::on_editSearch_textChanged() // search text changed
{
    refresh_filelist_filtering();
    if (ui->editSearch->text().length()==0) check_download_path();
}

void MainWindow::on_comboSortType_currentIndexChanged(int a) // new type of sorting
{
    refresh_filelist();
    refresh_filelist_filtering();
    check_download_path();
}

void MainWindow::on_actionSettings_Menu_triggered()
{
    SettingsWindow window(this);
    window.setModal(false);
    window.exec();
}

void MainWindow::on_actionQuit_triggered()
{
    qapp_->quit();
}

void MainWindow::on_actionAbout_triggered()
{
    AboutWindow about(this);
    about.setModal(false);
    about.exec();
}

void MainWindow::on_actionStatusbar_toggled(bool view_statusbar)
{
    settings->setShow_statusbar(view_statusbar);
    if(view_statusbar)
        ui->statusBar->show();
    else
        ui->statusBar->hide();
}

void MainWindow::customContextMenuRequested(QPoint pos)
{
    QModelIndex index = ui->listVideoQueue->indexAt(pos);

    QMenu *menu=new QMenu(this);
    menu->addAction("Toggle download Video+Audio/Audio only", this, SLOT(toggle_download_format()));

    menu->popup(ui->listVideoQueue->viewport()->mapToGlobal(pos));
}

void MainWindow::toggle_download_format()
{
    QModelIndexList indexes = ui->listVideoQueue->selectionModel()->selectedIndexes();
    QListWidgetItem *item;

    foreach(QModelIndex index, indexes)
    {
        item = ui->listVideoQueue->item(index.row());
        int item_key = item->data(Qt::UserRole).toInt();
        queue_items[item_key].toggleFormat();
        create_item_title_from_its_data(item);
    }
}

void MainWindow::delete_file_from_disk()
{
    if (ui->listVideos->currentRow()==-1) return;

    QMessageBox message;
    if (message.question(this, "Confirm File Delete", "Are you sure you want to delete the\nselected file from disk?",
                         QMessageBox::Yes | QMessageBox::No, QMessageBox::No)==QMessageBox::Yes)
    {
        QListWidgetItem* item = ui->listVideos->currentItem();
        QString item_text = item->text();
        fix_download_path(); // <- possibly deselects the current item - that's why it's right there.
        QFile file (ui->editDownloadPath->text() + item_text);

        if (file.remove())
        {
            QMessageBox response;
            response.setText("The file was deleted.");
            response.exec();
            refresh_filelist();
            refresh_filelist_filtering();
        }
        else
        {
            QMessageBox response;
            response.setText("Error: File could not be deleted!");
            response.exec();
        }
    }
}

void MainWindow::shortcut_delete()
{
    if (ui->listVideoQueue->hasFocus())
        delete_selected_item_on_queue();
    else if (ui->listVideos->hasFocus())
        delete_file_from_disk();
}

void MainWindow::on_btnShowStatus_clicked()
{
    if (ui->stackedQueueInfoOptions->currentIndex()!=0)
    {
        ui->stackedQueueInfoOptions->setCurrentIndex(0);
        ui->stackedQueueInfoOptions->show();
        return;
    }

    if (ui->stackedQueueInfoOptions->isHidden())
        ui->stackedQueueInfoOptions->show();
    else ui->stackedQueueInfoOptions->hide();

}

void MainWindow::on_btnShowOptions_clicked()
{
    if (ui->stackedQueueInfoOptions->currentIndex()!=1)
    {
        ui->stackedQueueInfoOptions->setCurrentIndex(1);
        ui->stackedQueueInfoOptions->show();
        return;
    }

    if (ui->stackedQueueInfoOptions->isHidden())
        ui->stackedQueueInfoOptions->show();
    else ui->stackedQueueInfoOptions->hide();

}

void MainWindow::on_checkAutoDownload_clicked()
{
    settings->setAuto_download(ui->checkAutoDownload->isChecked());
}
