#include "mediaitem.h"

QString MediaItem::getJsonElement(QString &allJsonContent, QString element)
{
    QRegularExpression exp ("(\""+element+"\":\\s\")(.*?)(\")");
    QRegularExpressionMatch match = exp.match(allJsonContent);
    return match.captured(2);
}

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
    QTextStream in (&qfile);
    in.setCodec("UTF-8");
    qfile.open(QIODevice::ReadOnly);
    QString allJsonContent (in.readAll());

    uploader = getJsonElement(allJsonContent,"uploader");
    title = getJsonElement(allJsonContent,"title");
    QList<int> tDate (giveDate(getJsonElement(allJsonContent,"upload_date")));
    upload_date.setDate(tDate.at(0),tDate.at(1),tDate.at(2));
}
