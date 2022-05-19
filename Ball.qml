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
            margins: selected ? 0 : 5
        }
        radius: width / 2
        color: display

        MouseArea {
            id: _clickableArea

            anchors.fill: _circle
            hoverEnabled: true
            onClicked: select()
        }

        Behavior on color {
            SequentialAnimation {
                id: _colorAnimation

                NumberAnimation {
                    id: _pause

                    duration: animDuration / 2
                }

                ColorAnimation {
                    id: _changeColor

                    duration: 1
                }
            }
        }
    }

    Behavior on opacity {
        SequentialAnimation {
            id: _opacityAnimation

            onStopped: {
                opacity = 1
            }

            NumberAnimation {
                id: _toInvisible

                property: "opacity"
                from: 1
                to: 0
                duration: animDuration / 2
            }

            NumberAnimation {
                id: _toVisible

                duration: animDuration / 2
            }
        }
    }

    InnerShadow {
        id: _glare

        anchors.fill: _circle
        horizontalOffset: 3
        verticalOffset: 3
        radius: 8
        samples: 17
        color: "dimgrey"
        source: _circle
    }
}
