import QtQuick 2.0
import QtGraphicalEffects 1.12

Item {
    id: root

    signal select

    Rectangle {
        id: _circle

        anchors {
            fill: parent
            margins: decoration ? 0 : 5
        }
        radius: width / 2
        color: display

        MouseArea {
            anchors.fill: _circle
            hoverEnabled: true
            onClicked: select()
            onExited: root.opacity = 1
            onEntered: root.opacity = 0.5
        }

        Behavior on color {

            ColorAnimation {
                easing.type: Easing.InExpo
                duration: animDuration - 100
            }
        }
    }

    InnerShadow {
        anchors.fill: _circle
        horizontalOffset: 3
        verticalOffset: 3
        radius: 8
        samples: 17
        color: "#80000000"
        source: _circle
    }
}
