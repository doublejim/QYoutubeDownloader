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
    format = 1 - format;
}
