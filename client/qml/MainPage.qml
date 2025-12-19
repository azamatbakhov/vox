import QtQuick
import QtQuick.Controls
import QtQuick.Controls.Material

Item {
    id: topLevelItem
    width: parent.width
    height: parent.height

    //! [orientation]
    readonly property bool inPortrait: window.width < window.height
    //! [orientation]

    ToolBar {
        id: overlayHeader

        z: 1
        width: parent.width
        parent: window.overlay

        Label {
            id: label
            anchors.centerIn: parent
            text: "Vox Messaging"
        }
    }

    Drawer {
        id: drawer

        y: overlayHeader.height
        width: window.width / 2
        height: window.height - overlayHeader.height

        modal: inPortrait
        position: inPortrait ? 0 : 1
        visible: !inPortrait

        ListView {
            id: listView
            anchors.fill: parent

            headerPositioning: ListView.OverlayHeader
            header: Pane {
                id: header
                z: 2
                width: parent.width

                contentHeight: logo.height

                Image {
                    id: logo
                    height: 64
                    width: parent.width
                    fillMode: Image.Stretch
                    source: "images/logo.png"
                }

                MenuSeparator {
                    parent: header
                    width: parent.width
                    anchors.verticalCenter: parent.bottom
                    visible: !listView.atYBeginning
                }
            }

            footer: ItemDelegate {
                id: footer
                text: qsTr("Footer")
                width: parent.width

                MenuSeparator {
                    parent: footer
                    width: parent.width
                    anchors.verticalCenter: parent.top
                }
            }

            model: 10

            delegate: ItemDelegate {
                text: qsTr("Title %1").arg(index + 1)
                width: parent.width
            }

            ScrollIndicator.vertical: ScrollIndicator { }
        }
    }

    SplitView {
        id: mainView

        anchors.fill: parent
        anchors.topMargin: overlayHeader.height

        ScrollView {
            id: chatList
            implicitHeight: topLevelItem.implicitHeight

            Label {
                font.pixelSize: 22
                width: parent.width
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                text: qsTr("Chat list")
            }
        }

        ScrollView {
            id: chatContent
            implicitHeight: topLevelItem.implicitHeight

 
            Label {
                width: parent.width
                wrapMode: Label.WordWrap
                text: qsTr("Chat content")
            }
        }
    }
}
