#ifndef OSD_H
#define OSD_H

#include <QDialog>
#include <QMenu>

namespace Ui {
class OSD;
}

class MainWindow;

class OSD: public QDialog
{
    Q_OBJECT

public:
    explicit OSD(MainWindow& main_window, QWidget *parent = 0);
    void update_progressbars(int audio, int video);
    void show(const QString &title);

    ~OSD();
public slots:
    void on_pushCancel_clicked();
    void customContextMenuRequested(QPoint pos);
    void save_position();
    void toggle_window_border();

private:
    Ui::OSD *ui;
    MainWindow& main_window;
};

#endif // OSD_H
