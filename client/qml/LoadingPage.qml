import QtQuick
import QtQuick.Controls

Item {
    width: parent.width
    height: parent.height

    Rectangle{
        id:bgRec
        anchors.fill: parent
        color: "#2C3E50"

        BusyIndicator {
            id: busyIndicator
            anchors.fill: parent
        }
    }
}
