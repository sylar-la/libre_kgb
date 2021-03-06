#ifndef DISCORDVOICECONNECTION_H
#define DISCORDVOICECONNECTION_H

#include "DiscordWebSocketConnection.h"

#include <QUdpSocket>

#include <sodium.h>

#include <opus.h>

#include <mutex>
#include <queue>

typedef struct
{
    float       data[960*2];
    uint32_t    size;
} SOpusFrame;

class DiscordVoiceConnection : public DiscordWebSocketConnection
{
    Q_OBJECT
public:
    DiscordVoiceConnection();

    void            voiceConnect();
    void            voiceDisconnect();

    void            onConnected();
    void            onDisconnected();
    void            onJSONPayloadReceived(QJsonObject &jsonObject);
    void            sendHeartbeat();

    void            setSpeaking(bool bSpeaking);

    void            sendVoiceData(const uint8_t* pData, uint32_t lLength);

    std::mutex      m_mtxOpusFramesMutex;
    std::queue<SOpusFrame*> m_qOpusFrames;
private slots:
    void            onUDPDataAvailable();
private:
    QString         m_strUDPServerIP;
    int             m_lUDPServerPort;
    int             m_lUDPSSRC;
    QUdpSocket*     m_pUDPSocket = nullptr;
    bool            m_bIPDiscoveryReceived = false;
    unsigned char   m_salsaSecretKey[32];
    uint16_t        m_lSendVoiceSequenceId = 1;
    uint32_t        m_lSendVoiceTimestamp = 1;
    OpusDecoder*    m_pOpusDecoder = nullptr;
    OpusEncoder*    m_pOpusEncoder = nullptr;
};

#endif
