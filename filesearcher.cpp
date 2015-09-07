#include "filesearcher.h"

void FileSearcher::search(QString searchDirectory, QStringList fileFilter)
{
    updatedDir = "";

    doingMyJob = true;
    abortSearch = false;

    QDir dir = searchDirectory;
    if (dir.exists()==false) return;

    MediaItemMap mediaItemMap;

    QDirIterator directories(searchDirectory, QDir::Files | QDir::NoSymLinks | QDir::NoDotAndDotDot, QDirIterator::Subdirectories);
    while(directories.hasNext())
        {
            directories.next();
            if (fileFilter.contains(directories.fileInfo().suffix(),Qt::CaseInsensitive)) // test each file suffix
            {
                QString path (directories.fileInfo().absoluteFilePath());
                QString suffix (directories.fileInfo().suffix());
                MediaItem newitem( path );
                newitem.fileName = directories.fileInfo().fileName();

                QFile metafile (path.left(path.length()-suffix.length()-1)+".info.json"); // construct the name the accompanying json-file would have had.
                if (metafile.exists()) // test if it exists. Note: Maybe there's a better way than this approach.
                {
                    newitem.jsonMetafile = metafile.fileName();
                }
                newitem.fillItUpJson();
                mediaItemMap.addItemGiveID(newitem);
            }
            if (abortSearch) {
                                doingMyJob=false;
                                search(updatedDir, updatedFilter);
                                return;
                             }
        }
    doingMyJob = false;
    emit sigMediaSearchComplete(mediaItemMap);
}

void FileSearcher::updateSearch(QString searchDirectory, QStringList fileFilter)
{
    updatedDir=searchDirectory;
    updatedFilter=fileFilter;
    abortSearch=true;
}
