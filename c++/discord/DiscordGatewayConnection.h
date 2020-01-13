#ifndef DISCORDGATEWAYCONNECTION_H
#define DISCORDGATEWAYCONNECTION_H

#include "DiscordWebSocketConnection.h"

class DiscordGatewayConnection : public DiscordWebSocketConnection
{
    Q_OBJECT
public:
    DiscordGatewayConnection();

    void        gatewayConnect();
    void        gatewayDisconnect();

    void        onJSONPayloadReceived(QJsonObject &jsonObject);
    void        sendHeartbeat();
private:
    QString     m_strUrl = "wss://gateway.discord.gg/?v=6&encoding=json";
    int         m_lSequenceNumber = 0;
};

#endif
