#ifndef FILESEARCH
#define FILESEARCH

#include <QObject>
#include <QDir>
#include <QFile>
#include <QDirIterator>
#include "mediaitemmap.h"

/* The FileSearcher fills up a MediaItemMap with all relevant files from a directory.
 * If a file has a JSON metadata file of the same name, it's combined with the MediaItem,
 * and the MediaItem is told to fill itself up with its metadata.
 */

class FileSearcher : public QObject
{
    Q_OBJECT
public:
    FileSearcher(){}
    ~FileSearcher(){}
    bool abortSearch = false;
    bool doingMyJob = false;
    QString updatedDir = "";
    QStringList updatedFilter;

public slots:
    void search(QString searchDirectory, QStringList fileFilter);
    void updateSearch(QString searchDirectory, QStringList fileFilter);

signals:
    void sigMediaSearchComplete(MediaItemMap a);
};

#endif // FILESEARCH
