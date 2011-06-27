import QtQuick 2.0

Rectangle {
    id: root

    property Item messageBubble
    property ListModel script: Conversation {}
    property int scriptIndex: 0
    property string sender: "Me"
    property int messageCounter: 0

    function send(message) {
        messageModel.set(messageModel.count - 1, {
             "sender": root.sender,
             "message": message,
             "avatar": "",
             "outbound": true,
             "time": Qt.formatTime(Date.now()),
             "delegateState": ""
        })
        visualModel.insert(visualModel.count, messageBubble)
        newMessage()
        messageView.positionViewAtEnd()
    }

    function newMessage() {
        messageModel.append({
            "sender": root.sender,
            "message": "",
            "avatar": "",
            "outbound": true,
            "time": "",
            "delegateState": "composing"
        })
        var bubble = visualModel.take(visualModel.count - 1, composer)
        messageBubble = bubble
        messageBubble.y = 0     // Override the position set by the view.
    }

    Component.onCompleted: newMessage()

    width: 480; height: 640

    gradient: Gradient {
        GradientStop { position: 0.0; color: "#000000" }
        GradientStop { position: 1.0; color: "#080808" }
    }

    ListView {
        id: messageView
        anchors {
            left: parent.left; top: parent.top; right: parent.right; bottom: composer.top
            topMargin: 1; bottomMargin: 2
        }
        spacing: 2

        cacheBuffer: 256

        model: VisualDataModel {
            id: visualModel

            model: ListModel { id: messageModel }
            delegate: Bubble {}
        }

        add: Transition {
            ParentAnimation {
                via: root
                NumberAnimation { properties: "y"; duration: 3000; easing.type: Easing.InOutQuad }
            }
        }
    }

    Timer {
        interval: 10000
        repeat: true
        running: true
        onTriggered: {
            var message = script.get(scriptIndex);

            messageModel.insert(messageModel.count - 1, {
                "sender": message.sender,
                "message": message.message,
                "avatar": message.avatar,
                "outbound": false,
                "time": Qt.formatTime(Date.now()),
                "delegateState": ""
            })

            scriptIndex = (scriptIndex + 1) % script.count
            interval = Math.random() * 30000
        }
    }

    Item {
        id: composer

        height: messageBubble != undefined ? messageBubble.contentHeight : 48
        anchors { left: parent.left; right: parent.right; bottom: parent.bottom }
    }
}
