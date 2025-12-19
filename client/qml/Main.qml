import QtQuick
import QtQuick.Controls

ApplicationWindow {
    width: 360
    height: 640
    visible: true
    title: qsTr("Vox Messaging")
    id: window

    StackView{
        id: contentFrame
        anchors.fill: parent
        initialItem: Qt.resolvedUrl("qrc:/LoadPage.qml")
        Component.onCompleted: {
            contentFrame.replace("qrc:/MainPage.qml")
        }
    }
}
