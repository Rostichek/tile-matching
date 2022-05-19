import QtQuick 2.0
import QtQuick.Controls 2.12
import BallsField 1.0

Popup {
    id: root

    anchors.centerIn: parent
    width: _window.width * 0.8
    height: _window.height * 0.2
    modal: true

    onClosed: _ballsModel.createBalls()

    Text {
        id: _victoryMessage

        anchors.centerIn: parent

        text: "There are no more swaps\n\nYour result is " + _ballsModel.score
              + " points by " + _ballsModel.steps + " steps"
    }
}
