#ifndef AUDIOSERVICE_H
#define AUDIOSERVICE_H

#include <portaudio.h>
#include <thread>
#include <vector>
#include <mutex>

#include <QtMultimedia/QAudioFormat>

class AudioService
{
public:
    AudioService();

    void                            initialize();
    void                            shutdown();

    PaStream*                       getStreamForSSRC(uint32_t lSSRC);
private:
    std::map<uint32_t, PaStream*>   m_mStreamsBySSRC;
    std::mutex                      m_mtxStreamsBySSRCMutex;
};

extern AudioService* sAudioService;

#endif
