#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QDebug>

#include <sodium.h>
#include <portaudio.h>

#include "c++/viewmodels/MainViewModel.h"

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;

    // Initialize Sodium.
    if(sodium_init() == -1)
    {
        qDebug() << "Failed to initialize Sodium";
        return 1;
    }

    // Initialize PortAudio.
    if(Pa_Initialize() != paNoError)
    {
        qDebug() << "Failed to initialize PortAudio";
        return 1;
    }

    // Create ViewModels.
    sMainViewModel = new MainViewModel();
    engine.rootContext()->setContextProperty("MainViewModel", sMainViewModel);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
