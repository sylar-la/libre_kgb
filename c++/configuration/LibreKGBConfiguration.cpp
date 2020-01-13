#include "LibreKGBConfiguration.h"

#include <QDebug>

LibreKGBConfiguration* sLibreKGBConfiguration = new LibreKGBConfiguration();

LibreKGBConfiguration::LibreKGBConfiguration() : m_qSettings("c:\\librekgb.cfg", QSettings::IniFormat)
{

}

void LibreKGBConfiguration::loadConfiguration()
{
    cfg_strBotToken = m_qSettings.value("bot_token").toString();

    qDebug() << "Bot Token: " << cfg_strBotToken;
}

void LibreKGBConfiguration::saveConfiguration()
{
    m_qSettings.setValue("bot_token", cfg_strBotToken);
}
