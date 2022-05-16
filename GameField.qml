import QtQuick 2.0
import BallsField 1.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Item {
    id: root

    property int expectedScreenSize: _balls.cellHeight * (_balls_model.rows / 2 + 0.5)
    property int animDuration: 300

    Rectangle {
        rotation: 180
        anchors.fill: parent
        color: "white"

        GameOverPopup {
            id: popup
        }

        GridView {
            id: _balls

            interactive: false
            anchors.fill: parent
            cellHeight: _window.width / _balls_model.columns
            cellWidth: cellHeight

            model: BallsModel {
                id: _balls_model
                // rows: Math.ceil(_window.height / _balls.cellHeight) + 1
                // the game will restart if you're trying to watch
                // upper than game field border
                // onRowsChanged: createBalls()
                onEndGame: popup.open()
            }

            delegate: Ball {
                width: _balls.cellHeight
                height: _balls.cellHeight

                onSelect: _balls_model.selectBall(index)
            }

            header: ScoreBar {
                id: _scoreBar
                width: root.width
                height: _balls.cellHeight / 2
            }

            move: _ballsTransition
        }

        Transition {
            id: _ballsTransition

            NumberAnimation {
                properties: "x, y"
                duration: animDuration
            }
        }
    }
}
