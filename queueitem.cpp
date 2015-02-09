#include "queueitem.h"

uint QueueItem::setFormat(uint i)
{
    format = i;
    return format;
}

QString QueueItem::setTitle(QString& newtitle)
{
    title = newtitle;
    return title;
}

void QueueItem::toggleFormat()
{
    if (format==0)
        format=1;
    else format=0;
}
