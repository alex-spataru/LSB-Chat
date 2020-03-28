/*
 * Copyright (c) 2020 Alex Spataru <https://github.com/alex-spataru>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

import QtQuick 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.2

Item {
    //
    // Let the bridge know that it can begin updating the UI elements
    //
    Component.onCompleted: CBridge.init()

    //
    // React to the events given the the QmlBridge class
    //
    Connections {
        target: CBridge

        //
        // New message received
        //
        onNewMessage: {
            // Blue for every peer
            var color = "#228"

            // Red for own user
            if (user === CBridge.userName)
                color = "#822"

            // Add user name
            chatView.appendText("<b>" + user + "</b>", color)

            // Add message
            chatView.appendText("&rarr;&nbsp;" + message, "#000")
        }

        //
        // Participant count changed
        //
        onNewParticipant: chatView.appendText(qsTr("%1 has joined the chat room").arg(name), "#444")
        onParticipantLeft: chatView.appendText(qsTr("%1 has left the chat room").arg(name), "#444")
    }

    //
    // Chat display control
    //
    TextField {
        id: chatroom
        readOnly: true

        //
        // Anchor to the left of the screen, and fill width & height
        //
        anchors {
            top: parent.top
            left: parent.left
            right: peers.left
            margins: app.spacing
            bottom: parent.bottom
        }

        //
        // Enable visible scrollbar
        //
        ScrollView {
            id: chatView
            clip: true
            anchors.fill: parent
            contentWidth: chatText.width
            anchors.margins: app.spacing
            ScrollBar.horizontal.policy: ScrollBar.AlwaysOff

            //
            // Appends the given @a text with the given @a color and autoscrolls the chat
            // log to the bottom
            //
            function appendText(text, color) {
                chatText.append("<font color=" + color + ">" + text + "</font>")
                ScrollBar.vertical.position = 1 - ScrollBar.vertical.size
            }

            //
            // Chat log display
            //
            TextEdit {
                id: chatText
                readOnly: true
                font.pixelSize: 12
                font.family: "Menlo"
                wrapMode: TextEdit.Wrap
                textFormat: TextEdit.RichText
                width: chatroom.width - 2 * app.spacing
            }
        }
    }

    //
    // Peer list
    //
    TextField {
        id: peers
        width: 192
        readOnly: true

        //
        // Anchor to the right of the screen and fill height
        //
        anchors {
            top: parent.top
            right: parent.right
            margins: app.spacing
            bottom: parent.bottom
        }

        //
        // Peer ListView
        //
        ListView {
            //
            // Generate the model with the data provided by the QML/C++ Bridge
            //
            id: peerList
            model: CBridge.peers

            //
            // Set layout
            //
            clip: true
            anchors.fill: parent
            anchors.margins: app.spacing

            //
            // Add a scrollbar
            //
            ScrollBar.vertical: ScrollBar {}

            //
            // Define how each individual element of the list should be
            //
            delegate: RowLayout {
                height: 36
                spacing: app.spacing / 2
                property color highlightColor: "#000"
                Behavior on highlightColor {ColorAnimation{duration: 100}}

                Connections {
                    target: CBridge
                    onNewMessage: {
                        if (user === modelData) {
                            if (user === CBridge.userName)
                                highlightColor = "#a44"
                            else
                                highlightColor = "#44a"

                            timer.start()
                        }
                    }
                }

                Timer {
                    id: timer
                    interval: 200
                    onTriggered: highlightColor = "#000"
                }

                RoundButton {
                    icon.width: 32
                    icon.height: 32
                    background: Item {}
                    icon.color: highlightColor
                    Layout.alignment: Qt.AlignVCenter
                    icon.source: "qrc:/icons/account_circle-24px.svg"
                }

                ColumnLayout {
                    spacing: app.spacing / 4
                    Layout.alignment: Qt.AlignVCenter

                    Label {
                        font.bold: true
                        font.pixelSize: 12
                        color: highlightColor
                        width: peerList.width
                        elide: Label.ElideRight
                        text: modelData.split("@")[0]
                    }

                    Label {
                        font.pixelSize: 10
                        color: highlightColor
                        width: peerList.width
                        elide: Label.ElideRight
                        text: modelData.split("@")[1]
                    }
                }
            }
        }
    }
}
