#include "DiscordVoiceConnection.h"
#include "DiscordClient.h"

#include <QDebug>
#include <QtEndian>

#include "../network/Buffer.h"
#include "../audio/AudioService.h"

DiscordVoiceConnection::DiscordVoiceConnection() : DiscordWebSocketConnection("Voice")
{
    // Create the UDP Socket
    m_pUDPSocket = new QUdpSocket(this);

    // Create the OPUS decoder.
    int opusError;
    m_pOpusDecoder = opus_decoder_create(48000, 2, &opusError);

    if(opusError < 0)
    {
        qDebug() << "OPUS Decoder Create Error: " << opusError;
        exit(1);
    }

    // Create the OPUS encoder.
    m_pOpusEncoder = opus_encoder_create(48000, 2, OPUS_APPLICATION_VOIP, &opusError);

    if(opusError < 0)
    {
        qDebug() << "OPUS Encoder Create Error: " << opusError;
        exit(1);
    }

    // Connect signal for incoming UDP datagrams to slot.
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

            Buffer ssrcBuffer;
            ssrcBuffer.WriteUInt16(0x01); // Request.
            ssrcBuffer.WriteUInt16(70);
            ssrcBuffer.WriteInt32(m_lUDPSSRC);
            for(int i = 0; i < 66; i++)
                ssrcBuffer.WriteUInt8(0x00);

            m_pUDPSocket->writeDatagram((char*)ssrcBuffer.GetBuffer(), ssrcBuffer.GetSize(), QHostAddress(m_strUDPServerIP), m_lUDPServerPort);

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

            //DUMP_JSON(jsonData)

            QJsonArray aSecretKeyValues = jsonData.take("secret_key").toArray();

            memset(m_salsaSecretKey, 0, 32);

            for(int i=0; i < aSecretKeyValues.size(); i++)
            {
                QJsonValue jsonVal = aSecretKeyValues.at(i);
                m_salsaSecretKey[i] = jsonVal.toInt();
            }

            qDebug() << "Salsa Key: " << QByteArray((char*)m_salsaSecretKey, 32).toHex();

            break;
        }
        case 5:
        {
            // Someone is speaking, or not.
            qDebug() << "VOICE Speaking";

            //DUMP_JSON(jsonData)

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

void DiscordVoiceConnection::setSpeaking(bool bSpeaking)
{
    QJsonObject speakingObject;
    speakingObject.insert("speaking", bSpeaking ? 1 : 0);
    speakingObject.insert("delay", 0);
    speakingObject.insert("ssrc", m_lUDPSSRC);

    sendSimplePacket(5, speakingObject);
}

void DiscordVoiceConnection::onUDPDataAvailable()
{
    Buffer datagramBuffer;
    datagramBuffer.Resize(m_pUDPSocket->pendingDatagramSize());

    QHostAddress sender;
    quint16 senderPort;

    m_pUDPSocket->readDatagram((char*)datagramBuffer.GetBuffer(), datagramBuffer.GetSize(), &sender, &senderPort);

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
        selectProtocolDataObject.insert("mode", "xsalsa20_poly1305");
        selectProtocolObject.insert("data", selectProtocolDataObject);

        //DUMP_JSON(selectProtocolObject)

        sendSimplePacket(1, selectProtocolObject);

        m_bIPDiscoveryReceived = true;
    }
    else
    {
        if(datagramBuffer.GetSize() >= 12 && datagramBuffer.Read<uint8_t>(0) == 0x90 && datagramBuffer.Read<uint8_t>(1) == 0x78)
        {
            //qDebug() << "Voice Packet " << QByteArray((char*)datagramBuffer.GetBuffer(), datagramBuffer.GetSize()).toHex();

            uint16_t lSequenceNumber = datagramBuffer.Read<uint16_t>(2);
            uint32_t lTimestamp = datagramBuffer.Read<uint32_t>(4);
            uint32_t lSSRC = datagramBuffer.Read<uint32_t>(8);

            //qDebug() << "SSRC " << lSSRC << " Seq " << lSequenceNumber << " Timestamp " << lTimestamp;

            unsigned char aXSalsaNonce[24] = { 0 };
            memcpy(aXSalsaNonce, datagramBuffer.GetBuffer(), 12); // header is 12 bytes from packet + 12 null bytes.

            unsigned char* pDecryptedData = new unsigned char[datagramBuffer.GetSize() - 12] { 0 };

            int decryptResult = crypto_secretbox_open_easy(pDecryptedData, datagramBuffer.GetBuffer() + 12, datagramBuffer.GetSize() - 12, aXSalsaNonce, m_salsaSecretKey);

            if(decryptResult != 0)
            {
                qDebug() << "Voice Packet decryption failed!";
            }
            else
            {
                qDebug() << "bandwidth " << opus_packet_get_bandwidth(pDecryptedData);
                qDebug() << "nbFrames " << opus_packet_get_nb_frames(pDecryptedData, datagramBuffer.GetSize() - 12);
                qDebug() << "samplesPerFrame " << opus_packet_get_samples_per_frame(pDecryptedData, 48000);

                float pcmFrames[960*2];
                int pcmFrameSize = opus_decode_float(m_pOpusDecoder, pDecryptedData + 8, datagramBuffer.GetSize() - 20, pcmFrames, 960, 0);

                if(pcmFrameSize < 0)
                {
                    qDebug() << "Opus Decode failed!";
                }
                else
                {
                    // These are always two 960-byte frames = 20 ms of Opus Audio at 48 kHz.
                    qDebug() << "Valid Opus Frame from available size: " << pcmFrameSize;

                    Pa_WriteStream(sAudioService->getStream(), pcmFrames, 960);

                    // Store the frame.
                    /**/

                    //m_mtxOpusFramesMutex.lock();
                    //{

                    //}
                    //m_mtxOpusFramesMutex.unlock();
                }
            }
        }
        else if(datagramBuffer.Read<uint8_t>(0) == 0xF8)
        {
            qDebug() << "Silence Packet";
        }
        else
        {
            // TODO make a stat of malformed packets %age.
            qDebug() << "MALFORMED VOICE PACKET !!!!";
            qDebug() << QByteArray((char*)datagramBuffer.GetBuffer(), datagramBuffer.GetSize()).toHex();
        }
    }
}

void DiscordVoiceConnection::sendVoiceData(const uint8_t* pData, uint32_t lLength)
{
    // Encode with Opus.
    unsigned char opusEncoded[1024];
    int opusEncodedLength = opus_encode(m_pOpusEncoder, (short*)pData, lLength, opusEncoded, 1024);

    qDebug() << "Opus Encoded Size: " << opusEncodedLength;

    Buffer sendBuffer;
    sendBuffer.WriteUInt8(0x90);
    sendBuffer.WriteUInt8(0x78);
    sendBuffer.WriteUInt16(m_lSendVoiceSequenceId);
    sendBuffer.WriteUInt32(m_lSendVoiceTimestamp);
    sendBuffer.WriteInt32(m_lUDPSSRC);

    unsigned char aNonce[24] { 0 };
    memcpy(aNonce, sendBuffer.GetBuffer(), 12);

    unsigned char aEncrypted[1024] { 0 };

    int encryptResult = crypto_secretbox_easy(aEncrypted, opusEncoded, opusEncodedLength, aNonce, m_salsaSecretKey);

    qDebug() << "Encrypt Result: " << encryptResult;

    sendBuffer.Append(aEncrypted, lLength);

    m_pUDPSocket->writeDatagram((char*)sendBuffer.GetBuffer(), sendBuffer.GetSize(), QHostAddress(m_strUDPServerIP), m_lUDPServerPort);

    if(m_lSendVoiceSequenceId == 0xFFFF)
        m_lSendVoiceSequenceId = 0;
    else
        m_lSendVoiceSequenceId++;

    m_lSendVoiceTimestamp += lLength;
}
