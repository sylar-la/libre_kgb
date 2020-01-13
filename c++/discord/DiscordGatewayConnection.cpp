#include "DiscordGatewayConnection.h"
#include "DiscordClient.h"
#include "../configuration/LibreKGBConfiguration.h"

#include <QJsonArray>

DiscordGatewayConnection::DiscordGatewayConnection() : DiscordWebSocketConnection("Gateway")
{

}

void DiscordGatewayConnection::gatewayConnect()
{
    wsConnect(QUrl(m_strUrl));
}

void DiscordGatewayConnection::gatewayDisconnect()
{
    wsDisconnect();
}

void DiscordGatewayConnection::onJSONPayloadReceived(QJsonObject &jsonObject)
{
    int lOpcode = jsonObject.take("op").toInt();
    QJsonObject jsonData = jsonObject.take("d").toObject();

    switch(lOpcode)
    {
        case 0:
        {
            QString eventName = jsonObject.take("t").toString();
            int lSequenceNumber = jsonObject.take("s").toInt();

            m_lSequenceNumber = lSequenceNumber;

            if(eventName == "READY")
            {
                qDebug() << "Ready Packet received.";

                heartbeatStart();
            }
            else if(eventName == "GUILD_CREATE")
            {
                //qDebug() << "Guild Create: " << QString::fromUtf8(QJsonDocument(jsonData).toJson(QJsonDocument::Compact));
                qDebug() << "Guild Name: " << jsonData.take("name").toString();
                qDebug() << "Guild ID: " << jsonData.take("id").toString();

                /*QJsonArray jsonChannelsArray = jsonData.take("channels").toArray();

                for(QJsonArray::iterator it = jsonChannelsArray.begin(); it != jsonChannelsArray.end(); ++it)
                {
                    QJsonObject channelObject = (*it).toObject();

                    qDebug() << "Channel " << channelObject.take("id").toString() << " / " << channelObject.take("name").toString();
                }*/

                // Connect to Voice - hardcoded to Libre Antenne / Radio.
                QJsonObject voiceStateUpdateObject;
                voiceStateUpdateObject.insert("guild_id", "261939673662750721");
                voiceStateUpdateObject.insert("channel_id", "278706666176774144");
                voiceStateUpdateObject.insert("self_mute", false);
                voiceStateUpdateObject.insert("self_deaf", false);

                sendSimplePacket(4, voiceStateUpdateObject);
            }
            else if(eventName == "VOICE_STATE_UPDATE")
            {
                qDebug() << "Voice State Update";

                //DUMP_JSON(jsonData)

                sDiscordClient->strSessionId = jsonData.take("session_id").toString();
            }
            else if(eventName == "VOICE_SERVER_UPDATE")
            {
                qDebug() << "Voice Server Update";

                //DUMP_JSON(jsonData)

                sDiscordClient->strVoiceEndpoint = "wss://" + jsonData.take("endpoint").toString().replace(":80", ":443");
                sDiscordClient->strVoiceToken = jsonData.take("token").toString();

                sDiscordClient->voiceConnect();
            }
            else if(eventName == "PRESENCE_UPDATE")
            {
                // TODO.
            }
            else if(eventName == "TYPING_START")
            {

            }
            else if(eventName == "MESSAGE_CREATE")
            {

            }
            else if(eventName == "MESSAGE_UPDATE")
            {

            }
            else if(eventName == "GUILD_MEMBER_UPDATE")
            {

            }
            else
            {
                qDebug() << "Received unknown event: " << eventName;
            }

            break;
        }
        case 10:
        {
            // Gateway Hello.
            int32_t lHeartbeatInterval = jsonData.take("heartbeat_interval").toInt();

            if(lHeartbeatInterval != 0)
            {
                m_lHeartbeatInterval = lHeartbeatInterval;
            }

            // Send Identify.
            QJsonObject identifyObject;
            identifyObject.insert("token", sLibreKGBConfiguration->cfg_strBotToken);

            QJsonObject propertiesObject;
            propertiesObject.insert("$os", "windows");
            propertiesObject.insert("$browser", "mylib");
            propertiesObject.insert("$device", "mylib");

            identifyObject.insert("properties", propertiesObject);

            sendSimplePacket(2, identifyObject);

            break;
        }
        default:
        {
            qDebug() << "Wss recv Opcode " << lOpcode << " Data " << QString::fromUtf8(QJsonDocument(jsonData).toJson(QJsonDocument::Compact));
        }
    }
}

void DiscordGatewayConnection::sendHeartbeat()
{
    QJsonObject heartbeatObject;
    heartbeatObject.insert("op", 1);

    if(m_lSequenceNumber == 0)
    {
        heartbeatObject.insert("d", QJsonValue(QJsonValue::Null));
    }
    else
    {
        heartbeatObject.insert("d", m_lSequenceNumber);
    }

    qDebug() << "gateway heartbeat: " << QString::fromUtf8(QJsonDocument(heartbeatObject).toJson(QJsonDocument::Compact));

    sendJSONPayload(heartbeatObject);
}
