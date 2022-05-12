import QtQuick 2.0
import QtGraphicalEffects 1.12

Item {
    id: root

    signal select

    Rectangle {
        id: circle

        anchors.fill: parent
        anchors.margins: decoration ? 0 : 5

        radius: width / 2

        color: display

        Behavior on color {

            ColorAnimation {
                duration: 1000
            }
        }

        MouseArea {
            anchors.fill: circle
            hoverEnabled: true
            onClicked: select()
            onExited: root.opacity = 1
            onEntered: root.opacity = 0.5
        }
    }

    InnerShadow {
        anchors.fill: circle
        horizontalOffset: 3
        verticalOffset: 3
        radius: 8
        samples: 17
        color: "#80000000"
        source: circle
    }
}
