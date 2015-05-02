#ifndef METADATACONNECTION
#define METADATACONNECTION

#include <QObject>
#include <QFile>
#include <QDate>
#include <QTime>
#include <QTextStream>
#include <QDebug>
#include <QRegularExpression>

/* If you provide a MediaItem with a JSON metadata file,
 * the object can retrieve its data.
 * The MediaItem should always be provided with a fullFilePath.
 */

class MediaItem
{
private:
    QList<int> giveDate(QString dateString);
public:
    MediaItem(){}
    MediaItem(QString path):fullFilePath(path),upload_date(){}
    MediaItem(QString path,QString metapath):fullFilePath(path),jsonMetafile(metapath){}
    ~MediaItem(){}

    int unique_id = 0;
    QString fullFilePath;
    QString fileName;
    QString jsonMetafile;
    QString title = "";
    QString uploader = "";
    QDate upload_date;
    QDate file_creation_date;
    QTime file_creation_time;
public slots:
    QString getJsonElement(QString& allJsonContent, QString element);
    void fillItUpJson();
};

#endif // METADATACONNECTION

