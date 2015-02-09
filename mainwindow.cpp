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

    load_settings();
    init_color_scheme();

    if (ui->editDownloadPath->text()=="")  ui->editDownloadPath->setText(".");

    QShortcut* shortcut = new QShortcut(QKeySequence(Qt::Key_Delete), ui->listVideoQueue);
    connect(shortcut, SIGNAL(activated()), this, SLOT(delete_selected_item_on_queue()));

    ui->listVideoQueue->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listVideoQueue, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenuRequested(QPoint)));
}

MainWindow::~MainWindow()
{
    save_settings();
    delete ui;
}

void MainWindow::load_settings()
{
    QSettings settings(QSettings::IniFormat,QSettings::UserScope, "Triple Jim Software", "QYoutubeDownloader");

    resize(settings.value("MainWindow/size", QSize(400, 400)).toSize());
    move(settings.value("MainWindow/pos", QPoint(200, 200)).toPoint());
    ui->editDownloadPath->setText(settings.value("main/DownloadPath").toString()); //);
    if (settings.value("main/ShowDetails").toBool()) ui->textDetails->show(); else ui->textDetails->hide();
    ui->editSearch->setText(settings.value("main/LastSearch").toString());
    ui->comboSortType->setCurrentIndex(settings.value("main/comboSortType").toInt());

    settingAlwaysHideDetails = settings.value("settings/AlwaysHideDetails").toBool();
    settingAutoDownload = settings.value("settings/AutoDownload").toBool();
    settingDarkStyle = settings.value("settings/DarkStyle").toBool();

    if (settingAlwaysHideDetails)
    {
        ui->btnToggleDetails->hide();
        ui->textDetails->hide();
    }
    // it doesn't yet load the download queue.
}

void MainWindow::save_settings()
{
    QSettings settings(QSettings::IniFormat,QSettings::UserScope, "Triple Jim Software", "QYoutubeDownloader");

    settings.setValue("MainWindow/size", size());
    settings.setValue("MainWindow/pos", pos());
    QDir dir (ui->editDownloadPath->text());

    if (dir.exists())
        settings.setValue("main/DownloadPath", ui->editDownloadPath->text());

    if (ui->textDetails->isHidden())
        settings.setValue("main/ShowDetails", false);
    else
        settings.setValue("main/ShowDetails", true);

    settings.setValue("main/LastSearch", ui->editSearch->text());
    settings.setValue("main/comboSortType", ui->comboSortType->currentIndex());

    settings.setValue("settings/AlwaysHideDetails", settingAlwaysHideDetails);
    settings.setValue("settings/AutoDownload", settingAutoDownload);
    settings.setValue("settings/DarkStyle", settingDarkStyle);
    // it doesn't yet save the download queue.
}

void MainWindow::init_color_scheme()
{
    if(settingDarkStyle)
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

// Requires the path to a media player.
// Right now, completely inflexible.
// Should be configured via a settings menu instead.
// But WORKS if you hard-code the path, etc.
//
// TODO: fix this method. Doesn't seem to work on linux.
// looks okay though.
void MainWindow::open_video()
{
    QListWidgetItem *item = ui->listVideos->currentItem();
    QString program = "/usr/bin/mpv";// f:/x64/Media Player Classic Home Cinema/mpc-hc64.exe"; // path to media player
    QFile check;
    check.setFileName(program);
    //if (check.exists()==false) return;
    QStringList arguments;
    arguments << '\"' + ui->editDownloadPath->text() + item->text() + '\"';
    QProcess* play_video = new QProcess();
    play_video->start(program, arguments);
    going_to_play_video = false; //(it already did at this point)
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
        dialog.setDirectory(".");

    QStringList filters;
    filters << "*.mp4";
    dialog.setNameFilters(filters);

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
                int i=newOutput.lastIndexOf("Destination:");
                if (i!=-1)
                {
                    uint current_item_key = ui->listVideoQueue->item(0)->data(Qt::UserRole).toUInt();
                    if (queue_items[current_item_key].format==0)  // Download Audio + video
                        download_progress = 2;
                    else // Download Audio
                        download_progress = 4;
                }
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
                    ui->progressVideo->setValue(value); // set progressbaren til den fundne værdi.
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
                    ui->progressAudio->setValue(value);
                    if (value==100) ++download_progress;
                }
                break;
                }
    }
}

void MainWindow::check_download_path()
{
    QDir dir = ui->editDownloadPath->text();

    if (dir.exists()==false || ui->editDownloadPath->text()=="")
    {
        complete_filelist.clear(); // must be done: in case the filter is changed, so it doesn't show any files from the previous path.
        ui->listVideos->clear();
        QListWidgetItem *item = new QListWidgetItem("[invalid folder]");
        ui->listVideos->addItem(item);
        ui->listVideos->setStyleSheet("font: italic 8pt \"MS Shell Dlg 2\"");
        return;
    }
    else if (complete_filelist.size()==0)
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
    ui->editDownloadPath->setText(dir); // puts back the clean version of the path.

    if (dir[dir.length()-1]!='/' &&
        dir[dir.length()-1]!='\\' )
        ui->editDownloadPath->setText(dir+'/');
}


void MainWindow::video_title_resolved(int item_number)
{
    QString new_title = youtube_dl->readAllStandardOutput().simplified();
    QListWidgetItem *list_item = ui->listVideoQueue->item(item_number);

    queue_items[item_number].title = new_title;
    create_item_title_from_its_data(list_item);
}

void MainWindow::start_resolving_video_title(int item_number, QString url)
{
    QString program = "youtube-dl";
    QStringList arguments;
    arguments << "-e" << url;

    youtube_dl = new QProcess(this);
    QSignalMapper *mapper = new QSignalMapper(this);

    connect (youtube_dl, SIGNAL(readyReadStandardOutput()), mapper, SLOT(map()));
    mapper->setMapping(youtube_dl, item_number);
    connect (mapper, SIGNAL(mapped(int)), this, SLOT(video_title_resolved(int)));

    youtube_dl->start(program, arguments);

    return;
}

void MainWindow::download_top_video()
{
    if (ui->listVideoQueue->count()==0) return; // quits if the list is empty

    QDir dir = ui->editDownloadPath->text();
    if (dir.exists()==false || ui->editDownloadPath->text()=="") { QMessageBox message; message.setWindowTitle("Invalid folder.");
                               message.setText("The download folder is invalid."); message.exec();
                               return; }
    fix_download_path(); // must happen AFTER the above check, because QDir::cleanPath causes a crash if the path is empty.

    QString program = "youtube-dl";
    QStringList arguments;

    QListWidgetItem* current_item = ui->listVideoQueue->item(0);
    uint current_item_key = current_item->data(Qt::UserRole).toUInt();
    ushort format = queue_items[current_item_key].format;

    QString format_to_download;
    switch (format)
    {
        case 1: format_to_download = "bestaudio"; break;
        default: format_to_download = "bestvideo+bestaudio"; break;
    }

    arguments << queue_items[current_item_key].url << "-o" << ui->editDownloadPath->text()+"%(uploader)s - %(title)s.%(ext)s" << "-f" << format_to_download;

    youtube_dl = new QProcess(this);
    connect (youtube_dl,SIGNAL(readyReadStandardOutput()), this, SLOT(refresh_interface()));
    connect (youtube_dl,SIGNAL(finished(int)),this,SLOT(downloading_ended(int)));

    ui->progressVideo->setValue(0);
    ui->progressAudio->setValue(0);
    ui->progressVideo->show();
    ui->progressAudio->show();
    ui->labelVideo->show();
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
    uint current_item_key = item->data(Qt::UserRole).toUInt();

    dialog1.load(queue_items[current_item_key].title , queue_items[current_item_key].format);
    // the solution to loading the settings should also be applied here!

    if (dialog1.exec())
    {
        QString url = dialog1.user_input;
        uint format = dialog1.format_to_download;
        queue_items[current_item_key].setTitle(url);
        queue_items[current_item_key].setFormat(format);
        create_item_title_from_its_data(item);
        ui->listVideoQueue->addItem(item);
    }

    if (settingAutoDownload && download_progress==0) download_top_video();
}

void MainWindow::delete_selected_item_on_queue()
{
    delete ui->listVideoQueue->currentItem();
}

void MainWindow::stop_downloading()
{
    youtube_dl->close();
    ui->btnStartDownload->setText("Resume");
    download_progress=0;
}

void MainWindow::create_item_title_from_its_data(QListWidgetItem* item)
{
    uint current_item_key = item->data(Qt::UserRole).toUInt();

    switch (queue_items[current_item_key].format)
    {
        case 0: item->setData(Qt::DisplayRole,queue_items[current_item_key].title+" | Video+Audio"); break;
        case 1: item->setData(Qt::DisplayRole,queue_items[current_item_key].title+" | Audio only"); break;
    }
}

void MainWindow::add_video_to_download_list(QString url, uint format)
{
    QListWidgetItem *item = new QListWidgetItem(url);

    item->setData(Qt::UserRole, queue_item_counter); // højeste nummer bliver til key

    queue_items[queue_item_counter].url = url;
    queue_items[queue_item_counter].title = url;
    queue_items[queue_item_counter].format = format;

    create_item_title_from_its_data(item);
    ui->listVideoQueue->addItem(item);

    start_resolving_video_title(queue_item_counter, url);

    ++queue_item_counter;

    if (settingAutoDownload && download_progress==0) download_top_video();
}

void MainWindow::refresh_filelist() // stores a list of filenames from the download path, in correct order, but doesn't show anything.
{
    QDir dir = ui->editDownloadPath->text();
    if (dir.exists()==false) return;

    QStringList filters;
    filters << "*.mp4";
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
    download_progress=0;
    refresh_filelist();
    refresh_filelist_filtering();
    QListWidgetItem *item = ui->listVideoQueue->item(0);
    delete item;
    --queue_item_counter;
    if (ui->listVideoQueue->count()>0) { download_top_video(); return; }
    ui->progressVideo->hide(); // will only hide progressbars if there are no more videos to download.
    ui->progressAudio->hide();
    ui->labelVideo->hide();
    ui->labelAudio->hide();
    ui->btnStartDownload->setText("Start downloading");
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

void MainWindow::on_btnToggleDetails_clicked() // toggle details
{
    if (ui->textDetails->isHidden()) ui->textDetails->show();
    else ui->textDetails->hide();
}

void MainWindow::on_btnAddVideoToQueue_clicked() // add video to download list
{
    DialogNewDownload dialog1;
    dialog1.setWindowTitle("Download this video");
    dialog1.setModal(true);
    if (dialog1.exec())
    {
        QString url = dialog1.user_input;
        uint format = dialog1.format_to_download;
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

void MainWindow::on_comboSortType_currentIndexChanged() // new type of sorting
{
    refresh_filelist();
    refresh_filelist_filtering();
    check_download_path();
}

void MainWindow::on_actionSettings_Menu_triggered()
{
    SettingsWindow window;
    window.setModal(false);
    window.load_settings(&settingAlwaysHideDetails, &settingAutoDownload, &settingDarkStyle);
    if (window.exec())
    {
        if (settingAlwaysHideDetails)
        {
            ui->btnToggleDetails->hide();
            ui->textDetails->hide();
        }
        else
            ui->btnToggleDetails->show();

        init_color_scheme();
    }
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
    QListWidgetItem *item;
    item = ui->listVideoQueue->item(ui->listVideoQueue->currentRow());
    uint current_item_key = item->data(Qt::UserRole).toUInt();
    queue_items[current_item_key].toggleFormat();
    create_item_title_from_its_data(item);
}
