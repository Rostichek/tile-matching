import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    id: _window

    width: 350
    height: 480
    visible: true
    title: qsTr("Hello World")

    GameField {
        anchors.fill: parent
    }
}
