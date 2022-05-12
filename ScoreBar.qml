import QtQuick 2.0
import BallsField 1.0
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.12

Rectangle {
    id: root

    rotation: 180
    color: "lightgray"

    RowLayout {
        anchors.fill: parent

        Button {
            Layout.preferredWidth: root.width / 3
            Layout.preferredHeight: root.height
            text: "Restart"
            onClicked: _balls_model.createBalls()
        }
        Item {
            id: _score
            Layout.preferredWidth: root.width / 3
            Layout.preferredHeight: root.height
            Text {
                anchors.fill: _score
                horizontalAlignment: Text.AlignHCenter
                anchors.topMargin: (root.height - font.pixelSize) / 3
                text: "Score: " + _balls_model.score
            }
        }
        Item {
            id: _steps
            Layout.preferredWidth: root.width / 3
            Layout.preferredHeight: root.height
            Text {
                anchors.fill: _steps
                horizontalAlignment: Text.AlignHCenter
                anchors.topMargin: (root.height - font.pixelSize) / 3
                text: "Steps: " + _balls_model.steps
            }
        }
    }
}
