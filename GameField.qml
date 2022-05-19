import QtQuick 2.0
import BallsField 1.0

Item {
    id: root

    readonly property real rowsWithContent: _ballsModel.rows / 2 + 0.5
    property int expectedScreenSize: _balls.cellHeight * rowsWithContent
    property int animDuration: 800

    Rectangle {
        id: _gameField

        rotation: 180
        anchors.fill: parent
        color: "white"

        GameOverPopup {
            id: _popup
        }

        GridView {
            id: _balls

            property bool isAllGroupsFound: false

            anchors.fill: parent
            cellHeight: _window.width / _ballsModel.columns
            cellWidth: cellHeight
            interactive: false

            model: _ballsModel

            delegate: Ball {
                width: _balls.cellHeight
                height: _balls.cellHeight

                onSelect: _ballsModel.selectBall(index)
            }

            header: ScoreBar {
                id: _scoreBar

                width: root.width
                height: _balls.cellHeight / 2
            }

            move: _ballsTransition
            displaced: _ballsTransition

            BallsModel {
                id: _ballsModel

                onEndGame: _popup.open()
            }

            transitions: Transition {
                id: _ballsTransition

                SequentialAnimation {
                    id: _moveAnimation

                    NumberAnimation {
                        id: _pause

                        duration: animDuration / 2
                    }

                    NumberAnimation {
                        id: _move

                        properties: "x, y"
                        duration: animDuration / 3
                    }
                }

                onRunningChanged: {
                    if (running) {
                        _balls.isAllGroupsFound = false
                        return
                    }

                    _balls.isAllGroupsFound = !_ballsModel.findAllBallsGroup()

                    if (_balls.isAllGroupsFound) {
                        _ballsModel.computeScore()
                        if (!_ballsModel.areThereMoreMoves()) {
                            _ballsModel.endGame()
                        }
                        return
                    }
                }
            }
        }
    }
}
