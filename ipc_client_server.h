#ifndef IPC_CLIENT_SERVER_H
#include <QObject>
#include <QLocalSocket>
#include <QLocalServer>
#include <QtWidgets>
#include <QDialog>
#include <QDebug>
#include <QStringList>
#include <mainwindow.h>

#define IPC_CLIENT_SERVER_H

class IPCClientServer : QObject
{
    Q_OBJECT

public:
    IPCClientServer(MainWindow* window, QString youtube_url = "");

private slots:
    void readAck();
    void readReply();
    void sendAck();
    void sendReply();
    void displayError(QLocalSocket::LocalSocketError socketError);
    void displayErrorReply(QLocalSocket::LocalSocketError socketError);

private:
    void sendMsg(QLocalServer * send_to, QString msg);
    void readMsg(QLocalSocket * socket_to_read);
    QLocalSocket * socket_ack;
    QLocalSocket * socket_reply;
    QLocalServer * server;
    QLocalServer * server_reply;
    const QString server_name = "QYoutubeDownloader";
    const QString server_name_reply = "QYoutubeDownloaderReply";
    QString youtube_url;
    quint16 blockSize;
    const bool activate_debugging = false;
    void debug(QString msg);
    MainWindow* window;
};

#endif // IPC_CLIENT_SERVER_H
