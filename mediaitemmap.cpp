#include "mediaitemmap.h"

void MediaItemMap::addItemGiveID(MediaItem item)
{
    item.unique_id=unique_id;
    map[unique_id]=item;
    ++unique_id;
}

void MediaItemMap::addItem(MediaItem item)
{
    map[item.unique_id]=item;
}

void MediaItemMap::removeItem(int item_id)
{
    map.remove(item_id);
}

MediaItem MediaItemMap::returnItem(int item_id)
{
    return map.value(item_id);
}

void MediaItemMap::clear()
{
    map.clear();
}

QList<MediaItem> MediaItemMap::returnAllItems()
{
    return map.values();
}

int MediaItemMap::returnItemsCount()
{
    return map.size();
}

MediaItemMap MediaItemMap::returnItemsSearchUploaderAndTitle(QString search)
{
    MediaItemMap results;
    foreach(MediaItem item, map.values())
    {
        bool found=true;
        foreach (QString searchPart, search.split(' ', QString::SkipEmptyParts))
        {
            if (item.uploader.indexOf(searchPart,0,Qt::CaseInsensitive)==-1 && item.title.indexOf(searchPart,0,Qt::CaseInsensitive)==-1)
            { found=false; }
        }
        if (found) results.addItem(item);
    }
    return results;
}

MediaItemMap MediaItemMap::returnItemsSearchDate(QString search, MediaItemMap::date_comparison comp)
{
    MediaItemMap results;
    switch(comp)
    {
        case equals:
        foreach(MediaItem item, map.values())
            if (item.upload_date.toString(Qt::ISODate)==search)
                results.addItem(item);
        break;
        case before:
        foreach(MediaItem item, map.values())
            if (item.upload_date.toString(Qt::ISODate)<search)
                results.addItem(item);
        break;
        case after:
        foreach(MediaItem item, map.values())
            if (item.upload_date.toString(Qt::ISODate)>search)
                results.addItem(item);
        break;
    }
    return results;
}
