#ifndef AUDIOSERVICE_H
#define AUDIOSERVICE_H

#include <portaudio.h>
#include <thread>

#include <QtMultimedia/QAudioFormat>

class AudioService
{
public:
    AudioService();

    void            initialize();
    void            shutdown();

    void            playTestSound();

    PaStream*       getStream();
private:
    void            audioThreadRoutine();
private:
    PaStream*       m_pStream;
    QAudioFormat    m_audioFormat;

    bool            m_bAudioThreadContinue = false;
    std::thread*    m_pAudioThread = nullptr;
};

extern AudioService* sAudioService;

int AudioServiceStreamCallback(const void *input, void *output, unsigned long frameCount, const PaStreamCallbackTimeInfo* timeInfo, PaStreamCallbackFlags statusFlags, void *userData);

#endif
