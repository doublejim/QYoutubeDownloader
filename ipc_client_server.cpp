#include "ipc_client_server.h"
#include <chrono>
#include <thread>

// Constructor
//
IPCClientServer::IPCClientServer(MainWindow* window, QString(youtube_url)) : QObject(0), window(window), youtube_url(youtube_url)
{
    socket_ack = new QLocalSocket(this);
    socket_reply = new QLocalSocket(this);

    connect(socket_ack, SIGNAL(readyRead()), this, SLOT(readAck()));
    connect(socket_ack, SIGNAL(error(QLocalSocket::LocalSocketError)),
            this, SLOT(displayError(QLocalSocket::LocalSocketError)));

    connect(socket_reply, SIGNAL(readyRead()), this, SLOT(readReply()));
    connect(socket_reply, SIGNAL(error(QLocalSocket::LocalSocketError)),
            this, SLOT(displayErrorReply(QLocalSocket::LocalSocketError)));

    blockSize = 0;
    socket_ack->connectToServer(server_name); // Genererer en fejl, hvis der ikke er en instans af programmet
                                              // Starter så en server, og bliver så "main" programmet.
}

// Private slots - bliver connected ovenfor
//
void IPCClientServer::sendAck()
{
    debug(__func__);
    sendMsg(server, QString("ack"));
    std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // Vent på at serveren starter en reply server, så der kan connectes til den
                                                                  // Ja, det er en træls løsning, men det er asynkront og kunne ikke finde på noget smartere.
    blockSize = 0;
    socket_reply->abort();
    socket_reply->connectToServer(server_name_reply);
}

void IPCClientServer::sendReply()
{
    debug(__func__);
    sendMsg(server_reply, QString("reply|" + youtube_url));
    window->do_not_save_settings = true;
    window->close();
}

void IPCClientServer::readAck()
{
    debug(__func__);
    readMsg(socket_ack);
}

void IPCClientServer::readReply()
{
    debug(__func__);
    readMsg(socket_reply);
}

void IPCClientServer::displayError(QLocalSocket::LocalSocketError socketError)
{
    switch (socketError)
    {
        case QLocalSocket::ServerNotFoundError: // Ingen server (instans af programmet) kører - start en ny.
                server = new QLocalServer(this);
                if (!server->listen(server_name)) //Kan teoretisk ske, hvis ingen server kører og der bliver startet flere meget hurtigt efter hinanden
                    qDebug() << "Unable to start server: " << server->errorString();
                else
                    connect(server, SIGNAL(newConnection()), this, SLOT(sendAck()));

            break;

        case QLocalSocket::ConnectionRefusedError:
            qDebug() << "Shouldn't happen";
            break;

        case QLocalSocket::PeerClosedError:
            break;

        default:
            qDebug() << "The following error occurred: " << socket_ack->errorString();
    }
}

void IPCClientServer::displayErrorReply(QLocalSocket::LocalSocketError socketError)
{
    switch (socketError)
    {
        case QLocalSocket::PeerClosedError:
            break;

        default:
            qDebug() << "The following error occurred: " << socket_ack->errorString();
    }
}

// Private methods
//
void IPCClientServer::sendMsg(QLocalServer * send_to, QString msg)
{
    debug(__func__);
    QByteArray block;
    QDataStream out(&block, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_5_3); // 5.4 not available at Nikolai
    out << (quint16)0;
    out << msg;
    out.device()->seek(0);
    out << (quint16)(block.size() - sizeof(quint16));

    QLocalSocket *clientConnection = send_to->nextPendingConnection();
    connect(clientConnection, SIGNAL(disconnected()),
            clientConnection, SLOT(deleteLater()));

    clientConnection->write(block);
    clientConnection->flush();
    clientConnection->disconnectFromServer();
}

void IPCClientServer::readMsg(QLocalSocket * socket_to_read)
{
    debug(__func__);
    QDataStream in(socket_to_read);
    in.setVersion(QDataStream::Qt_5_3); // 5.4 not available at Nikolai

    if (blockSize == 0) {
        if (socket_to_read->bytesAvailable() < (int)sizeof(quint16))
            return;
        in >> blockSize;
    }

    if (in.atEnd())
        return;

    QString recv;
    in >> recv;

    if (recv == QString("ack"))
    {
        debug("ack");
        window->hide();
        server_reply = new QLocalServer(this);
        server_reply->listen(server_name_reply);
        connect(server_reply, SIGNAL(newConnection()), this, SLOT(sendReply()));
    } else
    {
        debug("reply: " + recv);
        QStringList data =  recv.split('|');
        if (data[0] == QString("reply"))
        {
            window->add_video_to_download_list(data[1]);
        } else
        {
            debug("Malformed reply!");
        }
    }
}

void IPCClientServer::debug (QString msg)
{
    if(activate_debugging)
        qDebug() << msg;
}
