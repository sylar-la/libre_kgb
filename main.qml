import QtQuick 2.9
import QtQuick.Window 2.2
import QtQuick.Controls 2.2

Window
{
    visible: true

    width: 640
    height: 480

    Component.onCompleted:
    {
        MainViewModel.initialize();
    }

    Component.onDestruction:
    {
        MainViewModel.shutdown();
    }

    Button
    {
        anchors.centerIn: parent

        width: 100
        height: 32

        text: "Transmit"

        onPressed:
        {
            MainViewModel.transmitStart();
        }

        onReleased:
        {
            MainViewModel.transmitStop();
        }
    }
}
