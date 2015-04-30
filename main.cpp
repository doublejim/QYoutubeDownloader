#include <QApplication>
#include "sharedmemory.h"
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    QString youtube_url;
    if(argc > 1)
        youtube_url = argv[1];

    SharedMemory ipc(&w);

    if(ipc.is_first_instance())
    {
        if(youtube_url != "")
            w.add_video_to_download_list_DefaultFormat(youtube_url);

        w.show();
    }
    else
    {
        ipc.share_link_with_first_instance(youtube_url);
    }

    return a.exec();
}

