#include "AudioService.h"
#include "../discord/DiscordClient.h"

#include <QtMultimedia/QAudioDeviceInfo>

#include <QDebug>

AudioService* sAudioService = new AudioService();

AudioService::AudioService()
{

}

void AudioService::playTestSound()
{

}

PaStream* AudioService::getStream()
{
    return m_pStream;
}

void AudioService::initialize()
{
    if(Pa_OpenDefaultStream(&m_pStream, 0, 2, paFloat32, 48000, 960 * 2, NULL, NULL) != paNoError)
    {
        qDebug() << "Failed to open play stream!";
    }

    if(Pa_StartStream(m_pStream) != paNoError)
    {
        qDebug() << "Failed to start play stream!";
    }

    /*m_pAudioThread = new std::thread([this]()
    {
        audioThreadRoutine();
    });*/
}

void AudioService::shutdown()
{
    Pa_StopStream(m_pStream);
}

void AudioService::audioThreadRoutine()
{
    m_bAudioThreadContinue = true;

    while(m_bAudioThreadContinue)
    {
        if(!sDiscordClient->getVoiceConnection()->m_qOpusFrames.empty())
        {
            SOpusFrame* pOpusFrame = sDiscordClient->getVoiceConnection()->m_qOpusFrames.front();

            if(Pa_WriteStream(m_pStream, pOpusFrame->data, 960 * 2) != paNoError)
            {
                qDebug() << "Failed to play frame!";
            }
            else
            {
                qDebug() << "Frame played, remaining " << sDiscordClient->getVoiceConnection()->m_qOpusFrames.size();
            }

            sDiscordClient->getVoiceConnection()->m_qOpusFrames.pop();
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(20));
    }
}

int AudioServiceStreamCallback(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData)
{
    qDebug() << "AudioServiceCallback frameCount" << frameCount;

    // For each callback, check if we have pending audio frames waiting processing.
    //sDiscordClient->getVoiceConnection()->m_mtxOpusFramesMutex.lock();
    //{

    //}
    //sDiscordClient->getVoiceConnection()->m_mtxOpusFramesMutex.unlock();



    return 0;
}
