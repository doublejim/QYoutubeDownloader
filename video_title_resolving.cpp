#include "video_title_resolving.h"

void VideoTitleResolving::begin_resolve(uint item_key, QString url) // it gets the data from the title_resolving_queue
{
    mutex__name_resolving_happening.lock();

    QString program = "youtube-dl";

    QStringList arguments;
    arguments << "-e" << url;

    youtube_dl = new QProcess(this);
    QSignalMapper *mapper = new QSignalMapper(this);

    connect (youtube_dl, SIGNAL(readyReadStandardOutput()), mapper, SLOT(map()));
    mapper->setMapping(youtube_dl, item_key);
    connect (mapper, SIGNAL(mapped(int)), this, SLOT(end_resolve(int)));

    youtube_dl->start(program, arguments);
}

void VideoTitleResolving::end_resolve(int item_key)
{
    QString title = youtube_dl->readAllStandardOutput().simplified();
    emit name_resolved(item_key, title);
    mutex__name_resolving_happening.unlock();
}
