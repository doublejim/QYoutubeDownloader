#ifndef SHAREDMEMORY_H
#define SHAREDMEMORY_H
#include <QSharedMemory>
#include <QLocalSocket>
#include <QLocalServer>
#include <QBuffer>
#include <QDebug>
#include <QString>
#include "mainwindow.h"

class SharedMemory : QObject
{
    Q_OBJECT

public:
    SharedMemory(MainWindow *main_window, QObject *parent = 0);
    void share_link_with_first_instance(QString &data);
    bool is_first_instance();

private slots:
    void read_link_from_shared_memory();
    void link_has_been_read();

private:
    // This is main_programs socket name, and the shared memory key
    const QString unique_identifier = "QYoutubeDownloader";
    // Used by the sharing program, to know when the main_progarm has finished reading the shared memory
    // so it can quit gracefully
    const QString unique_reply_identifier = "QYoutubeDownloaderReply";

    QLocalServer main_program;
    QLocalServer wait_for_main_program;

    QLocalSocket notify_main_program;
    QLocalSocket notify_sharing_program;

    QSharedMemory shared_memory;

    MainWindow *main_window_;
};

#endif // SHAREDMEMORY_H
