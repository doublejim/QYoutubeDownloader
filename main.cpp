#include "mainwindow.h"
#include <QApplication>
#include <ipc_client_server.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w(&a);
    w.show();

    QString youtube_url;
    if(argc > 1)
    {
        youtube_url = argv[1];
        w.add_video_to_download_list(youtube_url, uint(0));
    }

    IPCClientServer clientserver(&w, youtube_url);

    return a.exec();
}
