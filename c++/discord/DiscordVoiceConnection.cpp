#include "DiscordVoiceConnection.h"
#include "DiscordClient.h"

#include <QDebug>
#include <QtEndian>

#include "../network/Buffer.h"

DiscordVoiceConnection::DiscordVoiceConnection() : DiscordWebSocketConnection("Voice")
{
    m_pUDPSocket = new QUdpSocket(this);

    connect(m_pUDPSocket, &QUdpSocket::readyRead, this, &DiscordVoiceConnection::onUDPDataAvailable);
}

void DiscordVoiceConnection::voiceConnect()
{
    if(!sDiscordClient->strVoiceEndpoint.isEmpty())
    {
        wsConnect(QUrl(sDiscordClient->strVoiceEndpoint));
    }
}

void DiscordVoiceConnection::voiceDisconnect()
{
    wsDisconnect();
}

void DiscordVoiceConnection::onConnected()
{

}

void DiscordVoiceConnection::onDisconnected()
{
    m_bIPDiscoveryReceived = false;
}

void DiscordVoiceConnection::onJSONPayloadReceived(QJsonObject &jsonObject)
{
    int lOpcode = jsonObject.take("op").toInt();
    QJsonObject jsonData = jsonObject.take("d").toObject();

    switch(lOpcode)
    {
        case 2:
        {
            // Voice Ready.
            qDebug() << "VOICE Ready";

            //DUMP_JSON(jsonData)

            m_strUDPServerIP = jsonData.take("ip").toString();
            m_lUDPServerPort = jsonData.take("port").toInt();
            m_lUDPSSRC = jsonData.take("ssrc").toInt();

            QByteArray ssrcBuffer = QByteArray::fromHex("0x01004600000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000");
            //qDebug() << "ssrc size " << ssrcBuffer.size();
            //m_pUDPSocket->bind(m_lUDPServerPort);
            m_pUDPSocket->writeDatagram(ssrcBuffer, QHostAddress(m_strUDPServerIP), m_lUDPServerPort);

            heartbeatStart();

            break;
        }
        case 3:
        {
            // Heartbeat request?
            break;
        }
        case 4:
        {
            // Session Description.
            qDebug() << "VOICE Session Description";

            QJsonArray aSecretKeyValues = jsonData.take("secret_key").toArray();

            m_salsaSecretKey.clear();
            for(QJsonArray::iterator it = aSecretKeyValues.begin(); it != aSecretKeyValues.end(); ++it)
            {
                QJsonValue jsonVal = *it;
                m_salsaSecretKey.append((char)jsonVal.toInt());
            }

            qDebug() << "Salsa Key: " << m_salsaSecretKey.toHex();

            break;
        }
        case 5:
        {
            // Someone is speaking, or not.
            qDebug() << "VOICE Speaking";

            DUMP_JSON(jsonData)

            break;
        }
        case 8:
        {
            // Voice Hello.
            qDebug() << "VOICE Hello";

            int32_t lHeartbeatInterval = jsonData.take("heartbeat_interval").toInt();

            if(lHeartbeatInterval != 0)
            {
                m_lHeartbeatInterval = lHeartbeatInterval;
            }

            // Send Identify.
            QJsonObject identifyObject;
            identifyObject.insert("server_id", "261939673662750721");
            identifyObject.insert("user_id", "665909113548439571");
            identifyObject.insert("session_id", sDiscordClient->strSessionId);
            identifyObject.insert("token", sDiscordClient->strVoiceToken);

            sendSimplePacket(0, identifyObject);

            break;
        }
        default:
        {
            qDebug() << "Received unknown VOICE packet " << lOpcode;

            DUMP_JSON(jsonData)
        }
    }
}

void DiscordVoiceConnection::sendHeartbeat()
{
    QJsonObject heartbeatPacket;

    heartbeatPacket.insert("op", 3);
    heartbeatPacket.insert("d", rand());

    sendJSONPayload(heartbeatPacket);
}

void DiscordVoiceConnection::onUDPDataAvailable()
{
    Buffer datagramBuffer;
    datagramBuffer.Resize(m_pUDPSocket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;

    m_pUDPSocket->readDatagram((char*)datagramBuffer.GetBuffer(),datagramBuffer.GetSize(),&sender,&senderPort);

    if(!m_bIPDiscoveryReceived)
    {
        QString strAddress = QString::fromUtf8((char*)datagramBuffer.GetBuffer() + 8);
        int lPort = ((datagramBuffer.Read<uint8_t>(72) & 0xFF) << 8) + (datagramBuffer.Read<uint8_t>(73) & 0xFF);

        qDebug() << "IP Discovery addr: " << strAddress << " port: " << lPort;

        // Send Select Protocol
        QJsonObject selectProtocolObject;
        selectProtocolObject.insert("protocol", "udp");
        QJsonObject selectProtocolDataObject;
        selectProtocolDataObject.insert("address", strAddress);
        selectProtocolDataObject.insert("port", lPort);
        selectProtocolDataObject.insert("mode", "xsalsa20_poly1305_lite");
        selectProtocolObject.insert("data", selectProtocolDataObject);

        //DUMP_JSON(selectProtocolObject)

        sendSimplePacket(1, selectProtocolObject);

        m_bIPDiscoveryReceived = true;
    }
    else
    {
        if(datagramBuffer.GetSize() >= 12 && datagramBuffer.Read<uint8_t>(0) == 0x90 && datagramBuffer.Read<uint8_t>(1) == 0x78)
        {
            uint16_t lSequenceNumber = datagramBuffer.Read<uint16_t>(2);
            uint32_t lTimestamp = datagramBuffer.Read<uint32_t>(4);
            uint32_t lSSRC = datagramBuffer.Read<uint32_t>(8);

            qDebug() << "VOICE Packet Seq " << lSequenceNumber << " Time " << lTimestamp << " SSRC " << lSSRC << " Size " << datagramBuffer.GetSize();
        }
    }
}
