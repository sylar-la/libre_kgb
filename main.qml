import QtQuick 2.9
import QtQuick.Window 2.2

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
}
