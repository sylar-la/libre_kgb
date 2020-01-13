#include "MainViewModel.h"

#include "../discord/DiscordClient.h"
#include "../configuration/LibreKGBConfiguration.h"

#include <QDebug>
#include <random>

MainViewModel* sMainViewModel = nullptr;

MainViewModel::MainViewModel() : QObject()
{

}

void MainViewModel::initialize()
{
    // Load Configuration.
    sLibreKGBConfiguration->loadConfiguration();

    // Create the Discord Client.
    sDiscordClient = new DiscordClient();
    sDiscordClient->mainConnect();

    // Create the Voice Transmit routine.
    m_pTransmitThread = new std::thread([this]()
    {
        this->transmitRoutine();
    });
}

void MainViewModel::shutdown()
{
    sDiscordClient->mainDisconnect();

    m_bTransmitThreadContinue = false;

    delete sDiscordClient;
}

void MainViewModel::transmitStart()
{
    m_bTransmit = true;

    sDiscordClient->getVoiceConnection()->setSpeaking(true);
}

void MainViewModel::transmitStop()
{
    m_bTransmit = false;

    sDiscordClient->getVoiceConnection()->setSpeaking(false);
}

void MainViewModel::transmitRoutine()
{
    m_bTransmitThreadContinue = true;

    while(m_bTransmitThreadContinue)
    {
        if(m_bTransmit)
        {
            // TODO Send Voice data.
            qDebug() << "Transmitting ...";

            // Generate random payload of sound data.
            unsigned char aData[136];
            for(int i = 0; i < 136; i++)
            {
                aData[i] = rand() % 256;
            }

            sDiscordClient->getVoiceConnection()->sendVoiceData(aData, 136);
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }
}
