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
            onClicked: balls_model.createBalls()
        }
        Item {
            id: score
            Layout.preferredWidth: root.width / 3
            Layout.preferredHeight: root.height
            Text {
                anchors.fill: score
                horizontalAlignment: Text.AlignHCenter
                anchors.topMargin: (root.height - font.pixelSize) / 3
                text: "Score: " + balls_model.score
            }
        }
        Item {
            id: steps
            Layout.preferredWidth: root.width / 3
            Layout.preferredHeight: root.height
            Text {
                anchors.fill: steps
                horizontalAlignment: Text.AlignHCenter
                anchors.topMargin: (root.height - font.pixelSize) / 3
                text: "Steps: " + balls_model.steps
            }
        }
    }
}
