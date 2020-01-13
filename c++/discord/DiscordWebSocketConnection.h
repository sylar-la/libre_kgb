#ifndef DISCORDWEBSOCKETCONNECTION_H
#define DISCORDWEBSOCKETCONNECTION_H

#include <QObject>
#include <QWebSocket>
#include <QJsonDocument>
#include <QJsonObject>

#include <thread>

#define DUMP_JSON(obj) \
    qDebug().noquote() << QString::fromUtf8(QJsonDocument(obj).toJson(QJsonDocument::Indented)); \

class DiscordWebSocketConnection : public QObject
{
    Q_OBJECT
public:
    DiscordWebSocketConnection(QString strName);

    bool                isConnected();
protected:
    void                wsConnect(QUrl url);
    void                wsDisconnect();

    void                heartbeatStart();
    void                heartbeatStop();

    void                sendJSONPayload(QJsonObject& jsonObject);
    void                sendSimplePacket(int opcode, QJsonObject& jsonObject);

    virtual void        onConnected() {}
    virtual void        onDisconnected() {}
    virtual void        onJSONPayloadReceived(QJsonObject& jsonObject) = 0;
    virtual void        sendHeartbeat() = 0;
private slots:
    void                onSocketConnected();
    void                onSocketDisconnected();
    void                onSocketTextMessageReceived(QString strMessage);
private:
    void                heartbeatRoutine();
private:
    bool                m_bConnected = false;
    QWebSocket          m_webSocket;
    QString             m_strName;
protected:
    int32_t             m_lHeartbeatInterval = -1;
    bool                m_bHeartbeatRunning = false;
    std::thread*        m_pHeartbeatThread = nullptr;
};

#endif
