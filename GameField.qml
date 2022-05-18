import QtQuick 2.0
import BallsField 1.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Item {
    id: root

    property int expectedScreenSize: _balls.cellHeight * (_balls_model.rows / 2 + 0.5)
    property int animDuration: 800

    Rectangle {
        rotation: 180
        anchors.fill: parent
        color: "white"

        GameOverPopup {
            id: popup
        }

        GridView {
            id: _balls

            property bool is_all_groups_found: false

            interactive: false
            anchors.fill: parent
            cellHeight: _window.width / _balls_model.columns
            cellWidth: cellHeight

            model: BallsModel {
                id: _balls_model
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
            displaced: _ballsTransition

            transitions: Transition {
                id: _ballsTransition

                SequentialAnimation {

                    NumberAnimation {
                        duration: animDuration / 2
                    }

                    NumberAnimation {
                        properties: "x, y"
                        duration: animDuration / 3
                    }
                }

                onRunningChanged: {
                    if (running) {
                        _balls.is_all_groups_found = false
                        return
                    }

                    _balls.is_all_groups_found = !_balls_model.findAllBallsGroup()

                    if (_balls.is_all_groups_found) {
                        if (!_balls_model.areThereMoreMoves())
                            _balls_model.endGame()
                        return
                    }
                }
            }
        }
    }
}
