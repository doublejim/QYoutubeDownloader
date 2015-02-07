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

void DialogNewDownload::on_btnboxOkCancel_accepted()
{
    user_input = ui->editUserInput->text();
}
