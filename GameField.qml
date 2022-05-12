import QtQuick 2.0
import BallsField 1.0

Item {
    id: root

    property int animDuration: 300

    Item {
        rotation: 180

        anchors.fill: parent

        GridView {
            id: balls

            interactive: false

            anchors.fill: parent

            cellHeight: _window.width / model.columns
            cellWidth: cellHeight

            model: BallsModel {
                id: balls_model
                rows: Math.ceil(_window.height / balls.cellHeight) + 1

                onRowsChanged: createBalls()
            }

            delegate: Ball {
                width: balls.cellHeight
                height: balls.cellHeight

                onSelect: balls_model.selectBall(index)
            }

            header: scoreBar

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

    Component {
        id: scoreBar

        Rectangle {
            id: bg
            width: root.width
            height: balls.cellHeight / 2

            rotation: 180

            color: "lightgray"

            Item {
                id: text_holder

                height: bg.height
                width: 70

                anchors {
                    left: bg.left
                    top: bg.top
                    leftMargin: 10
                    topMargin: (bg.height - score.font.pixelSize) / 3
                }

                Text {
                    id: score

                    anchors.fill: parent

                    text: "Score: " + balls_model.score + "\tSteps: " + balls_model.steps
                }
            }
        }
    }
}
