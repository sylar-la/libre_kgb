#include "MainViewModel.h"

#include "../discord/DiscordClient.h"
#include "../configuration/LibreKGBConfiguration.h"

#include <QDebug>

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
}

void MainViewModel::shutdown()
{
    sDiscordClient->mainDisconnect();

    delete sDiscordClient;
}
