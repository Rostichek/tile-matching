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

        GameOverPopup {
            id: popup
        }

        GridView {
            id: _balls

            interactive: false
            anchors.fill: parent
            cellHeight: _window.width / model.columns
            cellWidth: cellHeight

            model: BallsModel {
                id: _balls_model
                rows: Math.ceil(_window.height / _balls.cellHeight) + 1

                onRowsChanged: createBalls()
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
