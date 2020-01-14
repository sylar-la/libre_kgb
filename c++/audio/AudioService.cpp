#include "AudioService.h"
#include "../discord/DiscordClient.h"

#include <QtMultimedia/QAudioDeviceInfo>

#include <QDebug>

AudioService* sAudioService = new AudioService();

AudioService::AudioService()
{

}

void AudioService::initialize()
{

}

void AudioService::shutdown()
{
    // Stop all active streams.
    m_mtxStreamsBySSRCMutex.lock();
    {
        for(auto kp : m_mStreamsBySSRC)
        {
            PaStream* pStream = kp.second;

            Pa_StopStream(pStream);
            Pa_CloseStream(pStream);
        }

        m_mStreamsBySSRC.clear();
    }
    m_mtxStreamsBySSRCMutex.unlock();
}

PaStream* AudioService::getStreamForSSRC(uint32_t lSSRC)
{
    PaStream* pResult = nullptr;

    if(m_mStreamsBySSRC.count(lSSRC) > 0)
    {
        pResult = m_mStreamsBySSRC.at(lSSRC);
    }
    else
    {
        PaStreamParameters streamOutputParameters;
        streamOutputParameters.device = Pa_GetDefaultOutputDevice();
        streamOutputParameters.channelCount = 2;
        streamOutputParameters.sampleFormat = paFloat32;
        streamOutputParameters.hostApiSpecificStreamInfo = nullptr;
        streamOutputParameters.suggestedLatency = Pa_GetDeviceInfo(streamOutputParameters.device)->defaultLowOutputLatency;

        if(Pa_OpenStream(&pResult, NULL, &streamOutputParameters, 48000, 960 * 2, paClipOff, NULL, NULL) != paNoError)
        {
            qDebug() << "Failed to open stream for SSRC " << lSSRC;

            return nullptr;
        }

        if(Pa_StartStream(pResult) != paNoError)
        {
            qDebug() << "Failed to start stream for SSRC " << lSSRC;

            Pa_CloseStream(pResult);

            return nullptr;
        }

        qDebug() << "Stream created for SSRC " << lSSRC;

        m_mtxStreamsBySSRCMutex.lock();
        {
            m_mStreamsBySSRC.insert(std::make_pair(lSSRC, pResult));
        }
        m_mtxStreamsBySSRCMutex.unlock();
    }

    return pResult;
}
