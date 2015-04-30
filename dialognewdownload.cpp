#include "dialognewdownload.h"
#include "ui_dialognewdownload.h"

DialogNewDownload::DialogNewDownload(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewDownload)
{
    ui->setupUi(this);
}

DialogNewDownload::~DialogNewDownload()
{
    delete ui;
}

void DialogNewDownload::load(QString url, int format_to_download, bool download_subtitles)
{
    ui->editUserInput->setText(url);
    if (format_to_download == 0)
        ui->radioVideoAudio->setChecked(true);
    else ui->radioAudioOnly->setChecked(false);
    if (download_subtitles == true)
        ui->checkDownloadSubs->setChecked(true);
    else ui->checkDownloadSubs->setChecked(false);
}

void DialogNewDownload::on_btnboxOkCancel_accepted()
{
    download_url = ui->editUserInput->text();

    if (ui->radioVideoAudio->isChecked())
         format_to_download = 0;
    else format_to_download = 1;

    if (ui->checkDownloadSubs->isChecked())
        download_subtitles = true;
    else download_subtitles = false;
}
