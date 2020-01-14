#ifndef DISCORD_CLIENT_H
#define DISCORD_CLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QSslConfiguration>
#include <QJsonDocument>
#include <QJsonObject>

#include <QtWebSockets/QtWebSockets>

#include "DiscordGatewayConnection.h"
#include "DiscordVoiceConnection.h"

class DiscordClient : public QObject
{
    Q_OBJECT
public:
    DiscordClient();
    ~DiscordClient();

    void                            mainConnect();
    void                            mainDisconnect();

    void                            voiceConnect();
    void                            voiceDisconnect();

    DiscordVoiceConnection*         getVoiceConnection();

    QString                         strSessionId;

    QString                         strVoiceToken;
    QString                         strVoiceEndpoint;
private:
    // Network Access Manager for Web Requests.
    QNetworkAccessManager*          m_pNetworkAccessManager = nullptr;
    // SSL configuration for QNetworkRequests.
    QSslConfiguration               m_sslConfiguration;
    // Connections to Discord.
    DiscordGatewayConnection*       m_pGatewayConnection;
    DiscordVoiceConnection*         m_pVoiceConnection;
};

extern DiscordClient* sDiscordClient;

#endif
