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

void DialogNewDownload::load(QString url, int format_to_download)
{
    ui->editUserInput->setText(url);
    if (format_to_download==0)
        ui->radioVideoAudio->setChecked(true);
    else ui->radioAudioOnly->setChecked(true);
}

void DialogNewDownload::on_btnboxOkCancel_accepted()
{
    user_input = ui->editUserInput->text();
    if (ui->radioVideoAudio->isChecked())
         format_to_download = 0;
    else format_to_download = 1;
}
