#ifndef QUEUEITEM_H
#define QUEUEITEM_H

#include <QString>

using namespace std;

class QueueItem
{
public:
    QString title = "";
    ushort format = 0;

    uint setFormat(uint i);
    QString setTitle(QString& newtitle);
    void toggleFormat();
};

#endif // QUEUEITEM_H
