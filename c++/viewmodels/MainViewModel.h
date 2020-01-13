#ifndef MAINVIEWMODEL_H
#define MAINVIEWMODEL_H

#include <QObject>
#include <thread>

class MainViewModel : public QObject
{
    Q_OBJECT
public:
    MainViewModel();

    Q_INVOKABLE
    void            initialize();

    Q_INVOKABLE
    void            shutdown();

    Q_INVOKABLE
    void            transmitStart();
    Q_INVOKABLE
    void            transmitStop();
private:
    void            transmitRoutine();
private:
    std::thread*    m_pTransmitThread = nullptr;
    bool            m_bTransmitThreadContinue = false;
    bool            m_bTransmit = false;
};

extern MainViewModel* sMainViewModel;

#endif
