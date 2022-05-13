import QtQuick 2.12
import QtQuick.Window 2.12

Window {
    id: _window

    minimumWidth: 350
    minimumHeight: _view.expectedScreenSize
    visible: true

    maximumHeight: minimumHeight
    maximumWidth: minimumWidth

    GameField {
        id: _view
        anchors.fill: parent
    }
}
