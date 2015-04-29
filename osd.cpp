#include "osd.h"
#include "ui_osd.h"
#include <mainwindow.h>

OSD::OSD(MainWindow &main_window, QWidget *parent) :
    main_window(main_window),
    QDialog(parent),
    ui(new Ui::OSD)
{
    ui->setupUi(this);
//    this->setWindowFlags(Qt::Popup);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(customContextMenuRequested(QPoint)));
    ui->progressAudio->installEventFilter(this);
    ui->progressVideo->installEventFilter(this);
    ui->labelAudio->installEventFilter(this);
    ui->labelVideo->installEventFilter(this);
    this->installEventFilter(this);
}

void OSD::update_progressbars(int audio, int video)
{
    ui->progressAudio->setValue(audio);
    ui->progressVideo->setValue(video);
}

void OSD::showOSD(const QString& title)
{
    Qt::WindowFlags flags = Qt::Dialog;
    if(main_window.settings.value("OSD/HideDecoration").toBool()) //->osd_hide_decoration())
    {
        flags |= Qt::FramelessWindowHint;
    }
    flags |= Qt::WindowStaysOnTopHint;
    this->setWindowTitle(title);
    this->setWindowFlags(flags);
    this->move(main_window.settings.value("OSD/Position").toPoint()); // I simple do not understand, why this doesn't work when toggling window border
    this->resize(main_window.settings.value("OSD/Size").toSize());
    this->show();
}

OSD::~OSD()
{
    delete ui;
}

void OSD::on_pushCancel_clicked()
{
    main_window.cancel_download();
    this->hide();
}

void OSD::customContextMenuRequested(QPoint pos)
{
    QMenu *menu = new QMenu(this);
    menu->addAction("Toggle window decoration", this, SLOT(toggle_window_border()));
    menu->addAction("Save position", this, SLOT(save_position()));

    menu->popup(this->cursor().pos());
}

void OSD::save_position()
{
    main_window.settings.setValue("OSD/Position", pos());
    main_window.settings.setValue("OSD/Size", size());
}

void OSD::toggle_window_border()
{
    save_position();
    main_window.settings.setValue("OSD/HideDecoration", !main_window.settings.value("OSD/HideDecoration").toBool() );
    showOSD(this->windowTitle());
}

bool OSD::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick)
    {
        this->hide();
        return true;
    }
    else
    {
        return false;
    }
}
