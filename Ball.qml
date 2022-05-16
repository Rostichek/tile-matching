import QtQuick 2.0
import QtGraphicalEffects 1.12

Item {
    id: root

    signal select

    opacity: !hidden

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
            //            onExited: root.opacity = 1
            //            onEntered: root.opacity = 0.5
        }

        Behavior on color {
            SequentialAnimation {
                NumberAnimation {
                    duration: animDuration / 2
                }
            }
        }
    }

    Behavior on opacity {

        SequentialAnimation {
            onStopped: {
                opacity = 1
            }

            NumberAnimation {
                property: "opacity"
                from: 1
                to: 0
                duration: animDuration / 2
            }

            NumberAnimation {
                duration: animDuration / 2
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
