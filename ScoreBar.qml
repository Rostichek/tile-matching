import QtQuick 2.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12
import BallsField 1.0

Rectangle {
    id: root

    rotation: 180
    color: "lightgray"

    RowLayout {
        id: _menu

        anchors.fill: parent

        Button {
            id: _restart

            Layout.preferredWidth: root.width / 3
            Layout.preferredHeight: root.height

            text: "Restart"

            onClicked: _ballsModel.createBalls()
        }
        Item {
            id: _scoreContainer

            Layout.preferredWidth: root.width / 3
            Layout.preferredHeight: root.height

            Text {
                id: _scoreText

                anchors.fill: _scoreContainer
                horizontalAlignment: Text.AlignHCenter
                anchors.topMargin: (root.height - font.pixelSize) / 3

                text: "Score: " + _ballsModel.score
            }
        }
        Item {
            id: _stepsContainer

            Layout.preferredWidth: root.width / 3
            Layout.preferredHeight: root.height

            Text {
                id: _stepsText

                anchors.fill: _stepsContainer
                horizontalAlignment: Text.AlignHCenter
                anchors.topMargin: (root.height - font.pixelSize) / 3

                text: "Steps: " + _ballsModel.steps
            }
        }
    }
}
