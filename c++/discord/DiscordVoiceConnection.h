#ifndef DISCORDVOICECONNECTION_H
#define DISCORDVOICECONNECTION_H

#include "DiscordWebSocketConnection.h"

#include <QUdpSocket>

class DiscordVoiceConnection : public DiscordWebSocketConnection
{
    Q_OBJECT
public:
    DiscordVoiceConnection();

    void        voiceConnect();
    void        voiceDisconnect();

    void        onConnected();
    void        onDisconnected();
    void        onJSONPayloadReceived(QJsonObject &jsonObject);
    void        sendHeartbeat();
private slots:
    void        onUDPDataAvailable();
private:
    QString     m_strUDPServerIP;
    int         m_lUDPServerPort;
    int         m_lUDPSSRC;
    QUdpSocket* m_pUDPSocket = nullptr;
    bool        m_bIPDiscoveryReceived = false;
    QByteArray  m_salsaSecretKey;
};

#endif
