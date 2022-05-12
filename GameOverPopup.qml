import QtQuick 2.0
import BallsField 1.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Popup {
    id: root

    anchors.centerIn: parent
    width: _window.width * 0.8
    height: _window.height * 0.2
    modal: true

    onClosed: _balls_model.createBalls()

    Text {
        anchors.centerIn: parent

        text: "There are no more swaps\n\nYour result is " + _balls_model.score
              + " points by " + _balls_model.steps + " steps"
    }
}
