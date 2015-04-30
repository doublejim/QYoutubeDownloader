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
    QString download_url;
    int format_to_download;
    bool download_subtitles = false;
    bool download_metadata = true;
    void load(QString url = "", int format_to_download = 0, bool download_subtitles = false, bool download_metadata = true);

private slots:
    void on_btnboxOkCancel_accepted();

private:
    Ui::DialogNewDownload *ui;
};

#endif // DIALOGNEWDOWNLOAD_H
