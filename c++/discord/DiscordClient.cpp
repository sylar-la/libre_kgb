#include "DiscordClient.h"

DiscordClient* sDiscordClient = nullptr;

DiscordClient::DiscordClient() : QObject()
{
    m_pGatewayConnection = new DiscordGatewayConnection();
    m_pVoiceConnection = new DiscordVoiceConnection();

    // REST.
    m_pNetworkAccessManager = new QNetworkAccessManager();

    m_sslConfiguration = QSslConfiguration::defaultConfiguration();
    m_sslConfiguration.setProtocol(QSsl::TlsV1_2);

    //connect(m_pNetworkAccessManager, SIGNAL (finished(QNetworkReply*)), this, SLOT (onRESTReplyReceived(QNetworkReply*)));
}

DiscordClient::~DiscordClient()
{
    if(m_pGatewayConnection)
        delete m_pGatewayConnection;
    if(m_pVoiceConnection)
        delete m_pVoiceConnection;
}

void DiscordClient::mainConnect()
{
    if(!m_pGatewayConnection->isConnected())
    {
        // Connect.
        m_pGatewayConnection->gatewayConnect();
    }
}

void DiscordClient::mainDisconnect()
{
    voiceDisconnect();

    if(m_pGatewayConnection->isConnected())
    {
        // Disconnect.
        m_pGatewayConnection->gatewayDisconnect();
    }
}

void DiscordClient::voiceConnect()
{
    if(!m_pVoiceConnection->isConnected())
    {
        // Connect.
        m_pVoiceConnection->voiceConnect();
    }
}

void DiscordClient::voiceDisconnect()
{
    if(m_pVoiceConnection->isConnected())
    {
        // Disconnect.
        m_pVoiceConnection->voiceDisconnect();
    }
}

DiscordVoiceConnection* DiscordClient::getVoiceConnection()
{
    return m_pVoiceConnection;
}
