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

void DialogNewDownload::load(QString url, int format_to_download, bool download_subtitles, bool download_metadata)
{
    ui->editUserInput->setText(url);
    if (format_to_download == 0)
        ui->radioVideoAudio->setChecked(true);
    else ui->radioAudioOnly->setChecked(true);

    ui->checkDownloadSubs->setChecked(download_subtitles);
    ui->checkDownloadMeta->setChecked(download_metadata);
}

void DialogNewDownload::on_btnboxOkCancel_accepted()
{
    download_url = ui->editUserInput->text();

    format_to_download = ui->radioVideoAudio->isChecked() ? 0 : 1;
    download_subtitles = ui->checkDownloadSubs->isChecked();
    download_metadata = ui->checkDownloadMeta->isChecked();
}
