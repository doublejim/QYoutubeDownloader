#include "queueitem.h"

int QueueItem::setFormat(int i)
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
    if (format == 2) format = 0;
    else ++format;
}
