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

GroupBox {
    id: peerList
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
    // Layout
    //
    ColumnLayout {
        spacing: app.spacing
        anchors.fill: parent

        //
        // Title
        //
        Label {
            font.bold: true
            font.pixelSize: 16
            text: qsTr("Connected Peers") + ":"
        }

        //
        // List
        //
        ListView {
            id: list
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
                width: peerList.width - 4 * app.spacing

                Connections {
                    target: CBridge
                    onNewMessage: {
                        dot.opacity = (user === modelData) ? 0.80 : 0
                        timer.start()
                    }
                }

                Timer {
                    id: timer
                    interval: 200
                    onTriggered: dot.opacity = 0
                }

                RoundButton {
                    icon.width: 32
                    icon.height: 32
                    background: Item {}
                    icon.color: palette.text
                    Layout.alignment: Qt.AlignVCenter
                    icon.source: "qrc:/icons/account_circle-24px.svg"
                }

                ColumnLayout {
                    spacing: app.spacing / 4
                    Layout.alignment: Qt.AlignVCenter

                    Label {
                        font.bold: true
                        font.pixelSize: 12
                        width: list.width
                        elide: Label.ElideRight
                        text: modelData.split("@")[0]
                    }

                    Label {
                        font.pixelSize: 10
                        width: list.width
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
