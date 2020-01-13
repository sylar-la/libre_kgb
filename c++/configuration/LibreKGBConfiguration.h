#ifndef LIBREKGBCONFIGURATION_H
#define LIBREKGBCONFIGURATION_H

#include <QSettings>

class LibreKGBConfiguration
{
public:
    LibreKGBConfiguration();

    QString     cfg_strBotToken;

    void        loadConfiguration();
private:
    void        saveConfiguration();

    QSettings   m_qSettings;
};

extern LibreKGBConfiguration* sLibreKGBConfiguration;

#endif
