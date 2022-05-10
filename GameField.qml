import QtQuick 2.0
import BallsField 1.0

Item {
    id: root

    property int animDuration: 300

    rotation: 180

    GridView {
        id: balls

        interactive: false

        anchors.fill: parent

        cellHeight: _window.width / model.columns
        cellWidth: cellHeight

        model: BallsModel {
            id: balls_model
            rows: Math.ceil(_window.height / balls.cellHeight) + 1
        }

        delegate: Ball {
            width: balls.cellHeight
            height: balls.cellHeight

            onSelect: balls_model.selectBall(index)
        }

        move: ballsTransition
    }

    Transition {
        id: ballsTransition

        NumberAnimation {
            properties: "x, y"
            duration: animDuration
        }
    }
}
