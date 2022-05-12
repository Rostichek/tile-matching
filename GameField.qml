import QtQuick 2.0
import BallsField 1.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Item {
    id: root

    property int animDuration: 500

    Item {
        rotation: 180

        anchors.fill: parent

        Popup {
            id: popup
            anchors.centerIn: parent
            width: _window.width * 0.8
            height: _window.height * 0.2
            modal: true

            Text {
                anchors.centerIn: parent
                text: "There are no more swaps\n\nYour result is " + balls_model.score
                      + " points by " + balls_model.steps + " steps"
            }

            onClosed: balls_model.createBalls()
        }

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
                onEndGame: popup.open()
            }

            delegate: Ball {
                width: balls.cellHeight
                height: balls.cellHeight

                onSelect: balls_model.selectBall(index)
            }

            header: ScoreBar {
                id: scoreBar
                width: root.width
                height: balls.cellHeight / 2
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
}
