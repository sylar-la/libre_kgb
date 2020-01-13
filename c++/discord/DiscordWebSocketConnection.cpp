#include "DiscordWebSocketConnection.h"

DiscordWebSocketConnection::DiscordWebSocketConnection(QString strName)
: m_strName(strName)
{
    connect(&m_webSocket, &QWebSocket::connected, this, &DiscordWebSocketConnection::onSocketConnected);
    connect(&m_webSocket, &QWebSocket::disconnected, this, &DiscordWebSocketConnection::onSocketDisconnected);
    connect(&m_webSocket, &QWebSocket::textMessageReceived, this, &DiscordWebSocketConnection::onSocketTextMessageReceived);

    connect(&m_webSocket, QOverload<QAbstractSocket::SocketError>::of(&QWebSocket::error), [&](QAbstractSocket::SocketError error)
    {
        qDebug() << "WSConnection" << this->m_strName << " error: " << m_webSocket.errorString();
    });
}

bool DiscordWebSocketConnection::isConnected()
{
    return m_bConnected;
}

void DiscordWebSocketConnection::wsConnect(QUrl url)
{
    m_webSocket.open(url);
}

void DiscordWebSocketConnection::wsDisconnect()
{
    m_webSocket.close();
}

void DiscordWebSocketConnection::heartbeatStart()
{
    if(!m_pHeartbeatThread)
    {
        m_pHeartbeatThread = new std::thread([this]()
        {
            this->heartbeatRoutine();
        });
    }
}

void DiscordWebSocketConnection::heartbeatRoutine()
{
    m_bHeartbeatRunning = true;

    while(m_bHeartbeatRunning)
    {
        // Send heartbeat packet.
        sendHeartbeat();

        std::this_thread::sleep_for(std::chrono::milliseconds(m_lHeartbeatInterval));
    }
}

void DiscordWebSocketConnection::heartbeatStop()
{
    m_bHeartbeatRunning = false;
}

void DiscordWebSocketConnection::sendJSONPayload(QJsonObject &jsonObject)
{
    QJsonDocument jsonDoc;
    jsonDoc.setObject(jsonObject);

    QString strPayload = QString::fromUtf8(jsonDoc.toJson(QJsonDocument::Compact));

    m_webSocket.sendTextMessage(strPayload);
}

void DiscordWebSocketConnection::sendSimplePacket(int opcode, QJsonObject &jsonObject)
{
    QJsonObject parentObject;

    parentObject.insert("d", jsonObject);
    parentObject.insert("op", opcode);

    sendJSONPayload(parentObject);
}

void DiscordWebSocketConnection::onSocketConnected()
{
    qDebug() << "WSConnection " << m_strName << ": connected";

    m_bConnected = true;

    onConnected();
}

void DiscordWebSocketConnection::onSocketDisconnected()
{
    qDebug() << "WSConnection " << m_strName << ": disconnected";

    heartbeatStop();

    m_bConnected = false;

    onDisconnected();
}

void DiscordWebSocketConnection::onSocketTextMessageReceived(QString strMessage)
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(strMessage.toUtf8());

    if(jsonDoc.isNull())
    {
        qDebug() << "WSConnection " << m_strName << ": jsonDoc.isNull!";
        qDebug() << strMessage;
        return;
    }

    if(!jsonDoc.isObject())
    {
        qDebug() << "WSConnection " << m_strName << ": jsonDoc is not an object!";
        qDebug() << strMessage;
        return;
    }

    QJsonObject jsonObject = jsonDoc.object();

    onJSONPayloadReceived(jsonObject);
}
