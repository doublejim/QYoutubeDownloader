#include "sharedmemory.h"

SharedMemory::SharedMemory(MainWindow *main_window, QObject *parent) :
      main_window_(main_window),
      QObject(parent),
      shared_memory(unique_identifier)
{}

bool SharedMemory::is_first_instance()
{
    bool first = false;

    if (main_program.listen(unique_identifier)) //This is first instance
    {
        connect(&main_program, SIGNAL(newConnection()), this, SLOT(read_link_from_shared_memory()));
        first = true; // This is first instance.
    }

    return first;
}

void SharedMemory::share_link_with_first_instance(QString &data)
{
    // Load into shared memory
    QBuffer buffer;
    buffer.open(QBuffer::ReadWrite);
    QDataStream out(&buffer);
    out << data;
    int size = buffer.size();

    if (shared_memory.create(size))
    {
        shared_memory.lock();
        char *to = (char*)shared_memory.data();
        const char *from = buffer.data().data();
        memcpy(to, from, qMin(shared_memory.size(), size));
        shared_memory.unlock();

        // Listing for mainprogram to finish reading
        if (wait_for_main_program.listen(unique_reply_identifier)) //This is first instance
        {
            connect(&wait_for_main_program, SIGNAL(newConnection()), this, SLOT(link_has_been_read()));

            // Now link is shared and reply server is listening, notify server to read it
            notify_main_program.connectToServer(unique_identifier);
        }
        else
        {
            //TODO: Create decent error handling
            qDebug() << "Can't wait for mainprogram. Address probably in use.";
        }
    }
    else
    {
        //TODO: Create decent error handling
        qDebug() << "Unable to create shared memory segment: "  << shared_memory.errorString();
    }
}

// This is read from the first instance (main_program),
// when a second instance has loaded the link into a shared piece of memory
//
void SharedMemory::read_link_from_shared_memory()
{
    if (!shared_memory.attach())
    {
        qDebug()<<"Unable to read link from shared memory! :-(";
        return;
    }

    QBuffer buffer;
    QDataStream in(&buffer);
    QString url;

    shared_memory.lock();
    buffer.setData((char*)shared_memory.constData(), shared_memory.size());
    buffer.open(QBuffer::ReadOnly);
    in >> url;
    shared_memory.unlock();

    shared_memory.detach();

    main_window_->add_video_to_download_list(url);
    notify_sharing_program.connectToServer(unique_reply_identifier);
}

void SharedMemory::link_has_been_read()
{
    main_window_->close();
}
