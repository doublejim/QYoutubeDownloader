#include "mediaitemmap.h"

void MediaItemMap::addItem(MediaItem item)
{
    map[unique_id]=item;
    ++unique_id;
}

void MediaItemMap::removeItem(int item_id)
{
    map.remove(item_id);
}

MediaItem MediaItemMap::returnItem(int item_id)
{
    return map.value(item_id);
}

QList<MediaItem> MediaItemMap::returnAllItems()
{
    QList<MediaItem> results;
    QMap <int, MediaItem>::iterator i = map.begin();
    while (i != map.end())
            {
                results.append(i.value());
                ++i;
            }
    return results;
}

MediaItemMap MediaItemMap::returnItemsSearchUploader(MediaItemMap* dataset, QString search)
{
    MediaItemMap results;
    foreach(MediaItem item,(*dataset).returnAllItems())
        if (item.uploader.indexOf(search,0,Qt::CaseInsensitive)!=-1)
            results.addItem(item);
    return results;
}

MediaItemMap MediaItemMap::returnItemsSearchTitle(MediaItemMap* dataset, QString search)
{
    MediaItemMap results;
    foreach(MediaItem item,(*dataset).returnAllItems())
        if (item.title.indexOf(search,0,Qt::CaseInsensitive)!=-1)
            results.addItem(item);
    return results;
}

MediaItemMap MediaItemMap::returnItemsSearchDate(MediaItemMap* dataset, QString search, MediaItemMap::date_comparison comp)
{ // not in use yet. probably useful!
    MediaItemMap results;
    switch(comp)
    {
        case equals:
        foreach(MediaItem item,(*dataset).returnAllItems())
            if (item.upload_date.toString(Qt::ISODate)==search)
                results.addItem(item);
        break;
        case before:
        foreach(MediaItem item,(*dataset).returnAllItems())
            if (item.upload_date.toString(Qt::ISODate)<search)
                results.addItem(item);
        break;
        case after:
        foreach(MediaItem item,(*dataset).returnAllItems())
            if (item.upload_date.toString(Qt::ISODate)>search)
                results.addItem(item);
        break;
    }
    return results;
}

void MediaItemMap::absorbMap(MediaItemMap* absorbMap)
{
   // not done yet. probably needed for advanced searching.
}
