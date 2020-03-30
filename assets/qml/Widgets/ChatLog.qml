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
import QtQuick.Controls 2.3

GroupBox {
    id: chatroom

    //
    // React to the events given the the QmlBridge class
    //
    Connections {
        target: CBridge

        onNewMessage: {
            var color = Qt.lighter("#d57d25")
            if (user == CBridge.userName)
                color = "#ff0088"

            chatView.appendText("<b>" + user + "</b>", color)
            chatView.appendText("&rarr;&nbsp;" + message, "#aaa")
        }

        onNewParticipant: {
            chatView.appendText("<i>" + qsTr("%1 has joined the chat room").arg(name) + "</i>",
                                Qt.lighter(palette.highlight))
        }

        onParticipantLeft: {
            chatView.appendText("<i>" + qsTr("%1 has left the chat room").arg(name) + "</i>",
                                Qt.lighter(palette.highlight))
        }
    }

    //
    // Set background rectangle
    //
    background: Rectangle {
        border.width: 1
        color: palette.base
        border.color: palette.alternateBase
    }

    //
    // Enable visible scrollbar
    //
    ScrollView {
        id: chatView
        clip: true
        anchors.fill: parent
        contentWidth: parent.width
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
            wrapMode: TextEdit.Wrap
            textFormat: TextEdit.RichText
            width: chatroom.width - 2 * app.spacing
            onLinkActivated: Qt.openUrlExternally(link)

            text: "<font color=#00ff00>" +
                  qsTr("<b>Welcome</b>, please note that <u>maximum file size is 10 KB</u>.") + "</font><br/><br/>"

            font.family: {
                switch (Qt.platform.os.toString()) {
                case "osx":
                    return "Menlo"
                case "linux":
                    return "Mono"
                case "windows":
                    return "Consolas"
                }
            }
        }
    }
}
