#ifndef MEDIAITEMMAP
#define MEDIAITEMMAP

#include <QObject>
#include <QMap>
#include <QList>
#include "mediaitem.h"

/*
 * The MediaItemMap is really a QMap-wrapper, designed for containing a set of MediaItems.
 * It provides options for searching through the MediaItem-data.
 *
 * */

class MediaItemMap
{
private:
    int unique_id = 0;
    QMap<int,MediaItem> map;
public:
    enum date_comparison {before,equals,after};
    MediaItemMap():map(){}
    ~MediaItemMap(){}

public slots:
    void addItem(MediaItem item);
    void removeItem(int item_id);
    void absorbMap(MediaItemMap *absorbMap);
    MediaItem returnItem(int item_id);
    QList<MediaItem> returnAllItems();
    MediaItemMap returnItemsSearchUploader(MediaItemMap *dataset, QString search);
    MediaItemMap returnItemsSearchTitle(MediaItemMap *dataset, QString search);
    MediaItemMap returnItemsSearchDate(MediaItemMap *dataset, QString search, MediaItemMap::date_comparison comp);
};

#endif // MEDIAITEMMAP

