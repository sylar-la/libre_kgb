QT += quick network websockets multimedia

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Refer to the documentation for the
# deprecated API to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS SODIUM_STATIC

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
        deps/libsodium/include \
        deps/opus/include \
        deps/portaudio/include

SOURCES += \
        c++/audio/AudioService.cpp \
        c++/configuration/LibreKGBConfiguration.cpp \
        c++/discord/DiscordClient.cpp \
        c++/discord/DiscordGatewayConnection.cpp \
        c++/discord/DiscordVoiceConnection.cpp \
        c++/discord/DiscordWebSocketConnection.cpp \
        c++/network/Buffer.cpp \
        c++/network/EndianUtility.cpp \
        c++/viewmodels/MainViewModel.cpp \
        main.cpp

RESOURCES += qml.qrc

# Additional import path used to resolve QML modules in Qt Creator's code model
QML_IMPORT_PATH =

# Additional import path used to resolve QML modules just for Qt Quick Designer
QML_DESIGNER_IMPORT_PATH =

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    c++/audio/AudioService.h \
    c++/configuration/LibreKGBConfiguration.h \
    c++/discord/DiscordClient.h \
    c++/discord/DiscordGatewayConnection.h \
    c++/discord/DiscordVoiceConnection.h \
    c++/discord/DiscordWebSocketConnection.h \
    c++/network/Buffer.h \
    c++/network/EndianUtility.h \
    c++/viewmodels/MainViewModel.h

LIBS += \
    $$PWD/deps/libsodium/lib/libsodium.lib \
    $$PWD/deps/opus/lib/opus.lib \
    $$PWD/deps/portaudio/lib/portaudio.lib \
    User32.lib
