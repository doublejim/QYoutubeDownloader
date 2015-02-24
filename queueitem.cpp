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
    format = 1 - format;
}
