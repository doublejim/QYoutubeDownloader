#include "mediaitem.h"

QList<int> MediaItem::giveDate(QString dateString) // the date is returned as a list of ints, for instance: { 2015, 01, 01 }
{
    QRegularExpression exp ("(\\d\\d\\d\\d)(\\d\\d)(\\d\\d)");
    QRegularExpressionMatch match = exp.match(dateString);
    QList<int> date;
    date.append(match.captured(1).toInt());
    date.append(match.captured(2).toInt());
    date.append(match.captured(3).toInt());
    return date;
}

void MediaItem::fillItUpJson()
{   
    if (jsonMetafile=="") // Json isn't here, so he can't give us all the nice information.
    { title=fileName; return; } // We'll have to make do with just the filename (*sigh*).

    QFile qfile(jsonMetafile);
    qfile.open(QIODevice::ReadOnly | QIODevice::Text);
    QByteArray jsonData=qfile.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    QJsonObject obj = doc.object();

    uploader = obj.value("uploader").toString();
    title = obj.value("title").toString();
    QList<int> tDate (giveDate(obj.value("upload_date").toString()));
    upload_date.setDate(tDate.at(0),tDate.at(1),tDate.at(2));
}
