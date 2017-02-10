#ifndef QUEUEITEM_H
#define QUEUEITEM_H

#include <QString>

using namespace std;

class QueueItem
{
public:
    enum DownloadFormat {WHOLE_VIDEO, SEPARATE_THEN_MUX, ONLY_AUDIO};
    QString title = "";
    QString url = "";
    ushort format = 0;
    bool downloadSubtitles = false;
    bool downloadMetadata = false;

    int setFormat(int i);
    QString setTitle(QString& newtitle);
    void toggleFormat();
};

#endif // QUEUEITEM_H
