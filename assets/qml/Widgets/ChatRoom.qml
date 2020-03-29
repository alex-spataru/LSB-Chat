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
import Qt.labs.settings 1.0

RowLayout {
    //
    // Let the bridge know that it can begin updating the UI elements
    //
    Component.onCompleted: CBridge.init()

    //
    // Set spacing between components
    //
    spacing: app.spacing

    //
    // Define sidebar size
    //
    property int sidebarSize: 346

    //
    // Set the preview URL to use
    //
    property string lsbImageUrl: "image://lsb/composite"

    //
    // Save/load settings
    //
    Settings {
        property alias password: passwordTf.text
        property alias previewMode: lsbPreviewModeBt.checked
    }

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
            var color = Qt.lighter("#d57d25")

            // Red for own user
            if (user === CBridge.userName)
                color = Qt.lighter(palette.highlight)

            // Add user name
            chatView.appendText("<b>" + user + "</b>", color)

            // Add message
            chatView.appendText("&rarr;&nbsp;" + message, "#aaa")
        }

        //
        // Participant count changed
        //
        onNewParticipant: {
            chatView.appendText("<i>" + qsTr("%1 has joined the chat room").arg(name) + "</i>",
                                palette.highlight)
        }

        onParticipantLeft: {
            chatView.appendText("<i>" + qsTr("%1 has left the chat room").arg(name) + "</i>",
                                palette.highlight)
        }
    }

    //
    // Chat display control
    //
    GroupBox {
        id: chatroom
        Layout.fillWidth: true
        Layout.fillHeight: true

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
            contentWidth: chatText.width
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
                onLinkActivated: Qt.openUrlExternally(link)
            }
        }
    }

    //
    // Peer list and controls
    //
    ColumnLayout {
        spacing: app.spacing
        Layout.fillHeight: true
        Layout.minimumWidth: sidebarSize
        Layout.maximumWidth: sidebarSize

        //
        // Peer list
        //
        GroupBox {
            id: sidebar
            Layout.fillWidth: true
            Layout.fillHeight: true
            Layout.minimumHeight: 96

            //
            // Set background rectangle
            //
            background: Rectangle {
                border.width: 1
                color: palette.base
                border.color: palette.alternateBase
            }

            //
            // Peer ListView
            //
            ColumnLayout {
                spacing: app.spacing
                anchors.fill: parent

                Label {
                    font.bold: true
                    font.pixelSize: 16
                    text: qsTr("Connected Peers") + ":"
                }

                ListView {
                    id: peerList
                    clip: true
                    model: CBridge.peers
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    ScrollBar.vertical: ScrollBar {}

                    //
                    // Define how each individual element of the list should be
                    //
                    delegate: RowLayout {
                        height: 36
                        spacing: app.spacing / 2
                        width: sidebarSize - 4 * app.spacing
                        property color highlightColor: palette.buttonText
                        Behavior on highlightColor {ColorAnimation{duration: 100}}

                        Connections {
                            target: CBridge
                            onNewMessage: {
                                if (user === modelData) {
                                    dot.opacity = 1

                                    if (user === CBridge.userName)
                                        highlightColor = Qt.lighter(palette.highlight)
                                    else
                                        highlightColor = Qt.lighter("#d57d25")

                                    timer.start()
                                }

                                else
                                    dot.opacity = 0
                            }
                        }

                        Timer {
                            id: timer
                            interval: 200
                            onTriggered: highlightColor = palette.buttonText
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

                        Item {
                            Layout.fillWidth: true
                        }

                        Rectangle {
                            id: dot
                            width: 12
                            height: 12
                            opacity: 0
                            color: "#00ff00"
                            radius: width / 2
                            Layout.alignment: Qt.AlignVCenter
                            Behavior on opacity { NumberAnimation{duration: 100}}
                        }
                    }
                }
            }
        }

        //
        // LSB image display & controls
        //
        GroupBox {
            id: controls
            Layout.fillWidth: true
            Layout.fillHeight: false

            //
            // Set background rectangle
            //
            background: Rectangle {
                border.width: 1
                color: palette.base
                border.color: palette.alternateBase
            }

            ColumnLayout {
                anchors.fill: parent

                //
                // Crypto config title
                //
                Label {
                    font.bold: true
                    font.pixelSize: 16
                    text: qsTr("Encryption Key") + ":"
                }

                //
                // Password controls
                //
                RowLayout {
                    spacing: app.spacing
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter

                    TextField {
                        id: passwordTf
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter
                        onTextChanged: CBridge.setPassword(text)
                        placeholderText: qsTr("Enter a password") + "..."
                        echoMode: visibleBt.checked ? TextInput.Normal : TextInput.Password

                        background: Rectangle {
                            border.width: 1
                            color: palette.alternateBase
                            border.color: Qt.lighter(palette.alternateBase)
                        }
                    }

                    Button {
                        id: visibleBt
                        checkable: true
                        icon.color: palette.buttonText
                        Layout.alignment: Qt.AlignVCenter
                        icon.source: checked ? "qrc:/icons/visibility_off-24px.svg" :
                                               "qrc:/icons/visibility-24px.svg"
                    }
                }

                //
                // Spacer
                //
                Item {
                    height: app.spacing
                }

                //
                // Crypto config title
                //
                Label {
                    font.bold: true
                    font.pixelSize: 16
                    text: qsTr("Controls") + ":"
                }

                //
                // Buttons
                //
                GridLayout {
                    id: gridLayout

                    columns: 4
                    rowSpacing: 0
                    Layout.fillWidth: true
                    columnSpacing: (sidebarSize - (itemWidth * columns)) / (columns * 2)

                    readonly property real itemWidth: (sidebarSize - 4 * app.spacing) / columns

                    Button {
                        id: lsbPreviewModeBt
                        icon.width: 24
                        icon.height: 24
                        icon.color: palette.buttonText
                        Layout.alignment: Qt.AlignVCenter
                        Layout.minimumWidth: gridLayout.itemWidth
                        Layout.maximumWidth: gridLayout.itemWidth
                        icon.source: checked ? "qrc:/icons/grain-24px.svg" :
                                               "qrc:/icons/show_chart-24px.svg"
                        checkable: true
                        onCheckedChanged: {
                            if (checked)
                                lsbImageUrl = "image://lsb/data"
                            else
                                lsbImageUrl = "image://lsb/composite"

                            image.source = ""
                            image.source = lsbImageUrl
                        }
                    }

                    Button {
                        icon.width: 24
                        icon.height: 24
                        icon.color: palette.buttonText
                        onClicked: CBridge.saveImages()
                        Layout.alignment: Qt.AlignVCenter
                        icon.source: "qrc:/icons/save-24px.svg"
                        Layout.minimumWidth: gridLayout.itemWidth
                        Layout.maximumWidth: gridLayout.itemWidth
                    }

                    Button {
                        icon.width: 24
                        icon.height: 24
                        icon.color: palette.buttonText
                        Layout.alignment: Qt.AlignVCenter
                        onClicked: CBridge.selectLsbImageSource()
                        icon.source: "qrc:/icons/image_search-24px.svg"
                        Layout.minimumWidth: gridLayout.itemWidth
                        Layout.maximumWidth: gridLayout.itemWidth
                    }

                    Button {
                        icon.width: 24
                        icon.height: 24
                        icon.color: palette.buttonText
                        Layout.alignment: Qt.AlignVCenter
                        onClicked: CBridge.extractInformation()
                        Layout.minimumWidth: gridLayout.itemWidth
                        Layout.maximumWidth: gridLayout.itemWidth
                        icon.source: "qrc:/icons/unarchive-24px.svg"
                    }

                    Label {
                        font.pixelSize: 9
                        wrapMode: Label.WordWrap
                        width: gridLayout.itemWidth
                        horizontalAlignment: Text.AlignHCenter
                        Layout.minimumWidth: gridLayout.itemWidth
                        Layout.maximumWidth: gridLayout.itemWidth
                        text: lsbPreviewModeBt.checked ? qsTr("Show Composite Image") :
                                                         qsTr("Show Differential Image")
                    }

                    Label {
                        font.pixelSize: 9
                        wrapMode: Label.WordWrap
                        width: gridLayout.itemWidth
                        text: qsTr("Save Output Images")
                        horizontalAlignment: Text.AlignHCenter
                        Layout.minimumWidth: gridLayout.itemWidth
                        Layout.maximumWidth: gridLayout.itemWidth
                    }

                    Label {
                        font.pixelSize: 9
                        wrapMode: Label.WordWrap
                        width: gridLayout.itemWidth
                        text: qsTr("Select Image Source")
                        horizontalAlignment: Text.AlignHCenter
                        Layout.minimumWidth: gridLayout.itemWidth
                        Layout.maximumWidth: gridLayout.itemWidth
                    }

                    Label {
                        font.pixelSize: 9
                        wrapMode: Label.WordWrap
                        width: gridLayout.itemWidth
                        text: qsTr("Extract Data from Image")
                        horizontalAlignment: Text.AlignHCenter
                        Layout.minimumWidth: gridLayout.itemWidth
                        Layout.maximumWidth: gridLayout.itemWidth
                    }
                }

                //
                // Spacer
                //
                Item {
                    height: 1
                }

                //
                // Checkbox
                //
                CheckBox {
                    id: generateLsbImagesCheck
                    checked: CBridge.generateImages
                    text: qsTr("Generate LSB images automatically")
                    onCheckedChanged: CBridge.enableGeneratedImages(checked)
                }

                //
                // Spacer
                //
                Item {
                    height: 1
                }

                //
                // Crypto config title
                //
                Label {
                    font.bold: true
                    font.pixelSize: 16
                    text: qsTr("Output Image Preview") + ":"
                }

                //
                // Image display
                //
                Rectangle {
                    border.width: 2
                    color: palette.base
                    border.color: palette.alternateBase
                    Layout.alignment: Qt.AlignCenter
                    Layout.minimumWidth: sidebarSize - 2 * app.spacing
                    Layout.maximumWidth: sidebarSize - 2 * app.spacing
                    Layout.maximumHeight: sidebarSize - 2 * app.spacing
                    Layout.minimumHeight: sidebarSize - 2 * app.spacing

                    Image {
                        id: image
                        cache: false
                        smooth: true
                        anchors.margins: 2
                        asynchronous: true
                        source: lsbImageUrl
                        anchors.fill: parent
                        fillMode: Image.PreserveAspectCrop

                        Connections {
                            target: CBridge
                            onLsbImageChanged: {
                                image.source = ""
                                image.source = lsbImageUrl
                            }
                        }
                    }
                }
            }
        }
    }
}
