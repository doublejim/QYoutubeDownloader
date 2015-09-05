#include "mainwindow.h"
#include "ui_mainwindow.h"

//using namespace std;

/* INCONSISTENCIES:
 * 1________________________________________________________________
 * There is an inconsistency when it comes to checking a setting:
 * It is some times checked by:      QSettings settings.value().
 * Other times it's checked directly on the state of the object:        if checkBox.isChecked()
 *
 * I suggest always checking directly on the state of the object (if there is a corresponding object, of course).
 * Then it's not required that the QSettings setting is changed while running the program.
 * That means: fever functions: less code.
 * 2__________________________________________________________________
 * Should class variables always be private?
 * For instance, in the QueueItem is currently used:     setFormat()
 * instead of working directly on the variables (and right now they are public).
 * I'm not sure which is best.
 *
 */

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    settings (QSettings::IniFormat, QSettings::UserScope, "QYoutubeDownloader", "config"),
    settingsI (&settings),
    osd_(new OSD(*this)),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->progressVideo->hide();
    ui->progressAudio->hide();
    ui->labelVideo->hide();
    ui->labelAudio->hide();

    ui->comboSubdirPattern->addItem("Uploader name", "%(uploader)s");
    ui->comboSubdirPattern->addItem("Playlist title", "%(playlist)s");

    settingsI.join("Main/DownloadPath", ui->editDownloadPath, ".");
    settingsI.join("Main/SearchUploader", ui->editFilter);
    settingsI.join("Main/SearchDate", ui->editSearchDate);
    settingsI.join("Main/SearchDateCombo", ui->comboSearchDate);
    settingsI.join("Main/OpenInPlayerAfterDownload", ui->checkOpenInPlayerAfterDownload);
    settingsI.join("Main/ExitWhenFinished", ui->checkExitWhenFinished);
    settingsI.join("Main/SaveToSubdir", ui->checkSaveToSubdir);
    settingsI.join("Main/SubdirPattern", ui->comboSubdirPattern);
    settingsI.join("Main/AutoDownload", ui->checkAutoDownload);
    settingsI.join("Main/DownloadSubtitles", ui->checkDownloadSubs);
    settingsI.join("Main/DownloadMetadata", ui->checkDownloadMeta);

    apply_settings_at_startup();

    // Media file searching.
    fsearch = new FileSearcher;
    fsearch->moveToThread(&fsearchThread);
    fsearchThread.start();
    qRegisterMetaType<MediaItemMap>("MediaItemMap");
    connect(this,&MainWindow::sigYouShouldSearchForMedia,fsearch, &FileSearcher::beginSearch);
    connect(fsearch,&FileSearcher::sigMediaSearchComplete,this,&MainWindow::receivedMediaFiles);

    // TODO: replace the OLD SIGNALS/SLOTS with the MODERN way of writing it:
    // like this: connect(item_from, &MainWindow::function_from, item_to, &OtherWindow::function_to);
    // then all connections are verified at compile-time!

    QShortcut* shortcut_del = new QShortcut(QKeySequence(Qt::Key_Delete),this);
    connect(shortcut_del, SIGNAL(activated()), this, SLOT(shortcut_delete()));

    QShortcut* shortcut_enter = new QShortcut(QKeySequence(Qt::Key_Return), ui->tableMedia);
    connect(shortcut_enter, SIGNAL(activated()), this, SLOT(on_tableMedia_doubleClicked()));

    QShortcut* shortcut_ctrl_v = new QShortcut(QKeySequence(tr("Ctrl+V")), ui->listVideoQueue);
    connect(shortcut_ctrl_v, SIGNAL(activated()),this,SLOT(listVideoQueue_paste()));

    connect(ui->listVideoQueue->model(), SIGNAL(rowsInserted(const QModelIndex&, int, int)), this, SLOT(autostart_download(const QModelIndex&, int, int)));

    ui->listVideoQueue->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listVideoQueue, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenuRequested(QPoint)));
    qRegisterMetaType<QVector<int> >("QVector<int>");

    on_editDownloadPath_textChanged();
}

MainWindow::~MainWindow()
{
    osd_->close();

    if(!do_not_save_settings)
    {
        save_settings();
        settingsI.saveSettingsFromGUI();
    }

    delete fsearch;
    fsearchThread.exit();
    fsearchThread.wait();

    delete ui;
}

void MainWindow::apply_settings_at_startup()
{
    if ( settings.value("MainWindow/size").toSize() != QSize() ) // size not set, let wm chose
        resize( settings.value("MainWindow/size").toSize() );
    if ( settings.value("MainWindow/position").toPoint() != QPoint() ) // pos not set, let wm chose
        move( settings.value("MainWindow/position").toPoint() );

    ui->stackedQueueInfoOptions->setCurrentIndex( settings.value("Main/StackedWidgetActivePage").toInt() );
    ui->actionOSD->setChecked( settings.value("OSD/Show").toBool() );
    ui->actionStatusbar->setChecked( settings.value("Menu/ShowStatusBar").toBool() );

    if (settings.value("Menu/ShowStatusBar").toBool())
        ui->statusBar->show();
    else
        ui->statusBar->hide();

    if (settings.value("Main/ExpandStatusAndSettings").toBool())
        ui->stackedQueueInfoOptions->show();
    else
        ui->stackedQueueInfoOptions->hide();

    ui->splitter->restoreState( settings.value("MainWindow/splitter").toByteArray() );
    ui->tableMedia->setColumnWidth(0, settings.value("MainWindow/Column0Size",60).toInt());
    ui->tableMedia->setColumnWidth(1, settings.value("MainWindow/Column1Size",200).toInt());
    ui->tableMedia->setColumnWidth(2, settings.value("MainWindow/Column2Size",60).toInt());

    // defaults:
    if (settings.value("Settings/Youtube-dlExecutable").toString()=="")
        settings.setValue("Settings/Youtube-dlExecutable", qApp->applicationDirPath()+"/youtube-dl");
    if (settings.value("Settings/FFMPEGPath").toString()=="")
    {
        #ifdef Q_OS_WIN
        settings.setValue("Settings/FFMPEGPath", qApp->applicationDirPath()+"/ffmpeg.exe");
        #else
        settings.setValue("Settings/FFMPEGPath", qApp->applicationDirPath()+"/ffmpeg");
        #endif
    }
    if (settings.value("Settings/OutputTemplate").toString()=="")
        settings.setValue("Settings/OutputTemplate", "%(uploader)s - %(title)s.%(ext)s");

    apply_settings_while_running();
}

// This function is called at startup and also from the settings window, whenever ok or apply is pressed.
void MainWindow::apply_settings_while_running()
{
    if (settings.value("Settings/HideStatusButton").toBool())
    {
        ui->btnShowStatus->hide();
        if (ui->stackedQueueInfoOptions->currentIndex()==0)
            ui->stackedQueueInfoOptions->setCurrentIndex(1);
    }
    else ui->btnShowStatus->show();

    init_color_scheme();
}

void MainWindow::save_settings() // the settings that the QSettingsInterface can't take care of.
{
    if (!settings.value("Settings/DoNotSaveSizeAndPosition").toBool())
    {
        settings.setValue("MainWindow/size", size());
        settings.setValue("MainWindow/position", pos());
    }
    settings.setValue("MainWindow/splitter", ui->splitter->saveState());

    settings.setValue("Main/ExpandStatusAndSettings", !ui->stackedQueueInfoOptions->isHidden() );
    settings.setValue("Main/StackedWidgetActivePage", ui->stackedQueueInfoOptions->currentIndex());

    settings.setValue("MainWindow/Column0Size", ui->tableMedia->columnWidth(0));
    settings.setValue("MainWindow/Column1Size", ui->tableMedia->columnWidth(1));
    settings.setValue("MainWindow/Column2Size", ui->tableMedia->columnWidth(2));
}

void MainWindow::init_color_scheme()
{
    if (settings.value("Settings/DarkStyle").toBool())
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
            qApp->setStyleSheet(ts.readAll());
        }
    }
    else qApp->setStyleSheet("fusion");
}

int MainWindow::default_format()
{
    if ( ui->radioAudioVideo->isChecked() )
        return 0;
    else
        return 1;
}

void MainWindow::cancel_download()
{
    ui->btnStartDownload->click();
    QListWidgetItem *item = ui->listVideoQueue->item(0);
    int item_key = item->data(Qt::UserRole).toInt();
    delete item;
    queue_items.remove(item_key);
}

void MainWindow::listVideoQueue_paste()
{
    const QClipboard *clipboard = QApplication::clipboard();
    const QMimeData *mimeData = clipboard->mimeData();

    QString text;

    if (mimeData->hasText())
    {
        text = mimeData->text();
        int format;
        if (ui->radioAudioVideo->isChecked())
            format=0;
        else format=1;
        add_video_to_download_list_DefaultFormat(text);
    }
}

// TODO: Make it possible to send arguments to player.
void MainWindow::play_video(QString fullFilePath)
{
    QFile file (fullFilePath);
    if (!file.exists()) return;

    QString program = settings.value("Settings/MediaPlayer").toString();

    QStringList arguments;
    if (settings.value("Settings/MediaPlayerArgs").toString() != "")
        arguments << settings.value("Settings/MediaPlayerArgs").toString() << file.fileName();
    else
        arguments << file.fileName();

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
    QString newOutput = youtube_dl->readAllStandardOutput(); // new output from youtube-dl.

    if (ui->textDetails->verticalScrollBar()->sliderPosition()==ui->textDetails->verticalScrollBar()->maximum())
    {
        ui->textDetails->append(newOutput.simplified());
        ui->textDetails->verticalScrollBar()->setSliderPosition(ui->textDetails->verticalScrollBar()->maximum()); // scroll down automatically.
    }
    else {
        ui->textDetails->append(newOutput.simplified());
    }

    switch (download_progress)
    {
        case 1: { // Initialize
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
                    }
                }
                break;
                }
        case 2: { // Downloading video
                int i=newOutput.lastIndexOf("[download]"); // look for text: "[download]".
                if (i!=-1)
                {
                    QTextStream stream (&newOutput);
                    stream.seek(i+10); // go to the number after the "[download]"-text.
                    float value;
                    stream >> value; // save the number.
                    ui->progressVideo->setValue(value); // set progressbar to the found value.
                    QStringList first = newOutput.split(" of ");
                    QStringList second = first[1].split(" at ");
                    ui->statusBar->showMessage("Downloading video: " + second[0]);
                    if (value==100) ++download_progress;
                }
                break;
                }
        case 3: { // Initialize
                ui->statusBar->showMessage("Initialising audio download...");
                int i=newOutput.lastIndexOf("Destination:");
                if (i!=-1)
                    ++download_progress;
                break;
                }
        case 4: { // Downloading audio.
                int i=newOutput.lastIndexOf("[download]");
                if (i!=-1)
                {
                    QTextStream stream (&newOutput);
                    stream.seek(i+10);
                    float value;
                    stream >> value;
                    ui->progressAudio->setValue(value);
                    QStringList first = newOutput.split(" of ");
                    QStringList second = first[1].split(" at ");
                    ui->statusBar->showMessage("Downloading audio: " + second[0]);
                    if (value==100) ++download_progress;
                }
                break;
                }
    }
    if ( settings.value("OSD/Show").toBool() )
        osd_->update_progressbars(ui->progressAudio->value(), ui->progressVideo->value());
    last_youtubedl_output.append( newOutput ); // fixed a bug by appending instead of equaling.
}

void MainWindow::download_top_video()
{
    if (ui->listVideoQueue->count()==0) return; // quits if the list is empty

    QDir dir = ui->editDownloadPath->text();
    if (dir.exists()==false || ui->editDownloadPath->text() == "") { QMessageBox message; message.setWindowTitle("Invalid folder.");
                               message.setText("The download folder is invalid."); message.exec();
                               return; }
    fix_download_path(); // must happen AFTER the above check, because QDir::cleanPath causes a crash if the path is empty.

    QString program = settings.value("Settings/Youtube-dlExecutable").toString();
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

    QString output_path = ui->editDownloadPath->text();
    if (ui->checkSaveToSubdir->isChecked())
    {
        output_path += ui->comboSubdirPattern->currentData().toString() + "/";
    }

    output_path += settings.value("Settings/OutputTemplate").toString();

    arguments << "-o" << output_path
              << "-f" << format_to_download
              << "--ffmpeg-location" << settings.value("Settings/FFMPEGPath").toString()
              << "--merge-output-format" << "mkv"
              << queue_items[current_item_key].url;

    if ( queue_items[current_item_key].downloadSubtitles )
        arguments << "--all-subs";
    if ( queue_items[current_item_key].downloadMetadata )
        arguments << "--write-info-json";

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
    if(settings.value("OSD/Show").toBool())
    {
        if(!ui->listVideoQueue->hasFocus() && !ui->btnAddVideoToQueue->hasFocus()) //TODO: Fix this line, its wrong
            osd_->showOSD("Downloading...");
    }
    ++download_progress;
    youtube_dl->start(program,arguments);
}

void MainWindow::on_listVideoQueue_doubleClicked() // edit item
{
    DialogNewDownload dialog;
    dialog.setWindowTitle("Download this video");
    dialog.setModal(true);
    QListWidgetItem *item = ui->listVideoQueue->item( ui->listVideoQueue->currentRow() );
    int current_item_key = item->data(Qt::UserRole).toInt();

    dialog.load(queue_items[current_item_key].title, queue_items[current_item_key].format, queue_items[current_item_key].downloadSubtitles, queue_items[current_item_key].downloadMetadata);

    if (dialog.exec())
    {
        queue_items[current_item_key].setTitle( dialog.download_url );
        queue_items[current_item_key].setFormat( dialog.format_to_download );
        queue_items[current_item_key].downloadSubtitles = dialog.download_subtitles;
        queue_items[current_item_key].downloadMetadata = dialog.download_metadata;
        create_item_title_from_its_data(item);
        ui->listVideoQueue->addItem(item);
    }

    if (settings.value("Main/AutoDownload").toBool() && download_progress==0) download_top_video();
}

void MainWindow::delete_selected_item_on_queue()
{
    if(ui->listVideoQueue->selectedItems().contains(ui->listVideoQueue->item(0)))
    {
        if(youtube_dl != nullptr)
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
    osd_->hide();
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
    // Use youtube-dl --flat-playlist %url% to find out how many videos it's going to resolve!
    // show this in the new-video dialog.
    try
    {
        QString program = settings.value("Settings/Youtube-dlExecutable").toString();

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
            queue_items[unique_item_key].format = ui->radioAudioVideo->isChecked() ? 0 : 1; // IT SHOULDN'T JUST TAKE DEFAULTS
            queue_items[unique_item_key].downloadSubtitles = ui->checkDownloadSubs->isChecked(); // IT SHOULDN'T JUST TAKE DEFAULTS
            queue_items[unique_item_key].downloadMetadata = ui->checkDownloadMeta->isChecked(); // IT SHOULDN'T JUST TAKE DEFAULTS
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
    if (ui->listVideoQueue->count() == 1 && settings.value("Main/AutoDownload").toBool() && download_progress==0)
        download_top_video();
}

void MainWindow::add_video_to_download_list(QString url, int format, bool download_subtitles, bool download_metadata)
{
    QListWidgetItem *item = new QListWidgetItem(url);

    if(!url.contains("youtube.com/playlist") && !url.contains("youtube.com/user") && !url.contains("youtube.com/channel"))
    {
        item->setData(Qt::UserRole, unique_item_key); // højeste nummer bliver til key

        queue_items[unique_item_key].url = url;
        queue_items[unique_item_key].title = url;
        queue_items[unique_item_key].format = format;
        queue_items[unique_item_key].downloadSubtitles = download_subtitles;
        queue_items[unique_item_key].downloadMetadata = download_metadata;

        create_item_title_from_its_data(item);
        ui->listVideoQueue->addItem(item);
    }

    QtConcurrent::run(this, &MainWindow::resolve_title, unique_item_key, url);

    ++unique_item_key;
}

void MainWindow::add_video_to_download_list_DefaultFormat(QString url)
{
    int format = 0;
    if (ui->radioAudioOnly->isChecked()) format = 1;

    add_video_to_download_list(url, format, ui->checkDownloadSubs->isChecked(), ui->checkDownloadMeta->isChecked());
}

void MainWindow::check_download_path()
{
    QDir dir = ui->editDownloadPath->text();

    if (!dir.exists() || ui->editDownloadPath->text() == "")
    {
        ui->tableMedia->clearContents();
        QTableWidgetItem* item = new QTableWidgetItem("[invalid folder]");
        ui->tableMedia->setRowCount(1);
        ui->tableMedia->setItem(0,1,item);
    }
}

void MainWindow::fix_download_path()
{
    QString dir = QDir::cleanPath(ui->editDownloadPath->text()); // saves a clean version of the path.
    if(dir == ".") dir = QDir::currentPath();

    ui->editDownloadPath->setText(dir + '/');
}

QList<QTableWidgetItem*> MainWindow::make_MediaList_row(MediaItem& mediaitem)
{
    QList<QTableWidgetItem*> newRow;

    // UPLOADER
    QTableWidgetItem* newuploader = new QTableWidgetItem("");
    newuploader->setData(Qt::DisplayRole, mediaitem.uploader);
    newuploader->setData(Qt::UserRole, mediaitem.unique_id);
    newRow.append(newuploader);

    // TITLE
    QTableWidgetItem* newtitle = new QTableWidgetItem("");
    newtitle->setData(Qt::DisplayRole, mediaitem.title);
    newtitle->setData(Qt::UserRole, mediaitem.unique_id);
    newRow.append(newtitle);

    // DATE
    QTableWidgetItem* newdate = new QTableWidgetItem("");
    newdate->setData(Qt::DisplayRole, mediaitem.upload_date.toString(Qt::ISODate));
    newdate->setData(Qt::UserRole,mediaitem.unique_id);
    newRow.append(newdate);

    return newRow;
}

void MainWindow::fillMediaList(MediaItemMap* itemmap)
{
    if (itemmap==nullptr) return;
    ui->tableMedia->clearContents();
    ui->tableMedia->setRowCount(0);
    int index = 0;
    bool sortingSetting = ui->tableMedia->isSortingEnabled();
    ui->tableMedia->setSortingEnabled(false);
    foreach(MediaItem mediaitem, itemmap->returnAllItems())
    {
        QList<QTableWidgetItem*> newitem = make_MediaList_row(mediaitem);
        ui->tableMedia->insertRow(ui->tableMedia->rowCount());
        ui->tableMedia->setItem(index, 0, newitem.at(0));
        ui->tableMedia->setItem(index, 1, newitem.at(1));
        ui->tableMedia->setItem(index, 2, newitem.at(2));
        ++index; 
    }
    ui->tableMedia->setSortingEnabled(sortingSetting);
}

void MainWindow::receivedMediaFiles(MediaItemMap itemmap)
{
    if (itemmap.returnItemsCount() == 0)
    {
        ui->tableMedia->clearContents();
        QTableWidgetItem* item = new QTableWidgetItem("[no media files in folder]");
        ui->tableMedia->setRowCount(1);
        ui->tableMedia->setItem(0,1,item);
        return;
    }
    allMedia=itemmap;
    currentMedia = new MediaItemMap(allMedia);
    fillMediaList(&allMedia);
    refresh_MediaList_filtering();
}

void MainWindow::refresh_MediaList()
{
    QDir dir = ui->editDownloadPath->text();
    ui->tableMedia->clearContents();
    if (!dir.exists()) return;
    allMedia.clear();
    QTableWidgetItem* item = new QTableWidgetItem("searching...");
    ui->tableMedia->setRowCount(1);
    ui->tableMedia->setItem(0,1,item);

    QStringList fileFilter;
    fileFilter << "mkv" << "m4a" << "mp4" << "mp3" << "ogg" << "flv" << "webm";
    emit sigYouShouldSearchForMedia(ui->editDownloadPath->text(),fileFilter);
}

void MainWindow::refresh_MediaList_filtering() // filters the videos (without searching the harddisk)
{
    if (currentMedia!=nullptr)
        delete currentMedia;
    currentMedia = new MediaItemMap(allMedia);
    if (ui->editFilter->text() != "") (*currentMedia)=(*currentMedia).returnItemsSearchUploaderAndTitle(ui->editFilter->text());
    if (ui->editSearchDate->text() != "") (*currentMedia)=(*currentMedia).returnItemsSearchDate(ui->editSearchDate->text(), MediaItemMap::date_comparison(ui->comboSearchDate->currentIndex()));

    fillMediaList(currentMedia);
}

void MainWindow::downloading_ended(int a) // delete top video, download next top video.
{
    if (download_progress!=5) return; // is this good?
    youtube_dl->close();
    youtube_dl->deleteLater();
    if (settings.value("OSD/Show").toBool())
        osd_->hide();
    ui->statusBar->showMessage("Downloading finished.");

    download_progress=0;

    QRegularExpression exp ("([ffmpeg])(.*?)(\")(.*?)(\")"); // the line will say: [ffmpeg] Merging formats into "f:/filename.mkv" or [ffmpeg] Correcting container in "f:/filename.m4a"
    QRegularExpressionMatch match = exp.match(last_youtubedl_output);
    // CREATE AND ADD AS MEDIAITEM
    if(match.captured(4) != "") // If file found, create MediaItem.
    {
        QString filepath (match.captured(4));
        MediaItem mediaitem (filepath);
        QFileInfo fileinfo (filepath);
        mediaitem.fileName = fileinfo.fileName();

        QFile metafile (filepath.left(filepath.length()-fileinfo.suffix().length()-1)+".info.json"); // construct the name the accompanying json-file would have had.
        if (metafile.exists())
        {
            mediaitem.jsonMetafile = metafile.fileName();
        }
        mediaitem.fillItUpJson();
        allMedia.addItemGiveID(mediaitem);
        refresh_MediaList_filtering();
    }
    // PLAY MEDIA AFTER DOWNLOAD
    if(ui->checkOpenInPlayerAfterDownload->isChecked() && match.captured(4) != "")
    {
        play_video(match.captured(4));
    }

    last_youtubedl_output.clear(); // important, because: The last_youtubedl_output should only contain output from the latest download!

    // DELETE QUEUE ITEM
    QListWidgetItem *item = ui->listVideoQueue->item(0);
    int item_key = item->data(Qt::UserRole).toInt();
    delete item;
    queue_items.remove(item_key);
    //
    if (ui->listVideoQueue->count()>0) { download_top_video(); return; }
    ui->progressVideo->hide(); // will only hide progressbars if there are no more videos to download.
    ui->progressAudio->hide();
    ui->labelVideo->hide();
    ui->labelAudio->hide();
    ui->btnStartDownload->setText("Start downloading");
    if(ui->checkExitWhenFinished->isChecked() && ui->listVideoQueue->count() == 0)
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
    DialogNewDownload dialog;
    dialog.setWindowTitle("Download this video");
    dialog.setModal(true);
    int format = 0;
    if (ui->radioAudioOnly->isChecked()) format = 1;

    dialog.load("", format, ui->checkDownloadSubs->isChecked(), ui->checkDownloadMeta->isChecked());
    if (dialog.exec())
    {
        add_video_to_download_list(dialog.download_url, dialog.format_to_download, dialog.download_subtitles, dialog.download_metadata);
    }
}

void MainWindow::on_editDownloadPath_textChanged() // download folder lineEdit box
{
    refresh_MediaList();
    check_download_path();
}

void MainWindow::on_actionSettings_Menu_triggered()
{
    SettingsWindow window(*this);
    window.setModal(true);
    window.exec();
}

void MainWindow::on_actionQuit_triggered()
{
    qApp->quit();
}

void MainWindow::on_actionAbout_triggered()
{
    AboutWindow about(this);
    about.setModal(false);
    about.exec();
}

void MainWindow::on_actionStatusbar_toggled(bool view_statusbar)
{
    settings.setValue("Menu/ShowStatusBar",view_statusbar);

    if(view_statusbar)
        ui->statusBar->show();
    else
        ui->statusBar->hide();
    this->repaint();
}

void MainWindow::on_actionOSD_toggled(bool show_osd)
{
    settings.setValue("OSD/Show",show_osd);
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
    if (ui->tableMedia->currentRow()==-1) return;

    QTableWidgetItem* item = ui->tableMedia->currentItem();
    int item_id = item->data(Qt::UserRole).toInt();

    QMessageBox message;
    if (message.question(this, "Confirm File Delete", "Are you sure you want to delete\n"+allMedia.returnItem(item_id).fileName+"\nfrom disk?",
                         QMessageBox::Yes | QMessageBox::No, QMessageBox::No)==QMessageBox::Yes)
    {
        QFile file (allMedia.returnItem(item_id).fullFilePath);

        if (file.remove())
        {
            QMessageBox response;
            response.setText("The file was deleted.");
            response.exec();
            ui->tableMedia->removeRow(ui->tableMedia->currentRow());
            allMedia.removeItem(item_id);
            currentMedia->removeItem(item_id);
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
    else if (ui->tableMedia->hasFocus())
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
    settings.setValue("Main/AutoDownload",ui->checkAutoDownload->isChecked());
}

void MainWindow::on_editFilter_textChanged(const QString &arg1)
{
    refresh_MediaList_filtering();
}

void MainWindow::on_editSearchDate_textChanged(const QString &arg1)
{
    refresh_MediaList_filtering();
}

void MainWindow::on_comboSearchDate_currentIndexChanged(int index)
{
    refresh_MediaList_filtering();
}

void MainWindow::on_tableMedia_doubleClicked() // doubleclicking on a media item.
{
    if (ui->tableMedia->currentRow()==-1) return;
    QTableWidgetItem* item = ui->tableMedia->currentItem();
    QString file (allMedia.returnItem(item->data(Qt::UserRole).toInt()).fullFilePath);
    play_video(file);
}


