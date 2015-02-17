#ifndef VIDEO_TITLE_RESOLVING_H
#define VIDEO_TITLE_RESOLVING_H

#include <QObject>
#include <QStringList>
#include <QMutex>
#include <QMutexLocker>
#include <QProcess>
#include <QSignalMapper>

using namespace std;

class VideoTitleResolving : public QObject
{
    Q_OBJECT

signals:
    void name_resolved(uint item_key, QString title);

public:
    VideoTitleResolving() {}

public slots:
    void begin_resolve(uint item_key, QString url);

private slots:
    void end_resolve(int item_key);

private:
    QMutex mutex__name_resolving_happening;
    QProcess* youtube_dl;
};

#endif // VIDEO_TITLE_RESOLVING_H
