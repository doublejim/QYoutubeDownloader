#ifndef DIALOGNEWDOWNLOAD_H
#define DIALOGNEWDOWNLOAD_H

#include <QDialog>

namespace Ui {
class DialogNewDownload;
}

class DialogNewDownload : public QDialog
{
    Q_OBJECT

public:
    explicit DialogNewDownload(QWidget *parent = 0);
    ~DialogNewDownload();
    QString user_input;
    ushort format_to_download;
    void load(QString url, ushort format_to_download);

private slots:
    void on_btnboxOkCancel_accepted();

private:
    Ui::DialogNewDownload *ui;
};

#endif // DIALOGNEWDOWNLOAD_H