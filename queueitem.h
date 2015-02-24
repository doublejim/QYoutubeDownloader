#ifndef QUEUEITEM_H
#define QUEUEITEM_H

#include <QString>

using namespace std;

class QueueItem
{
public:
    QString title = "";
    QString url = "";
    ushort format = 0;

    int setFormat(int i);
    QString setTitle(QString& newtitle);
    void toggleFormat();
};

#endif // QUEUEITEM_H
