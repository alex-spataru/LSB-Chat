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
import QtQuick.Window 2.0
import QtQuick.Layouts 1.0
import QtQuick.Controls 2.0

import "Widgets" as Widgets

Window {
    //
    // Window title
    //
    id: window
    title: qsTr("Preferences")

    //
    // Window geometry
    //
    minimumWidth: 720
    minimumHeight: 520

    //
    // Background widget
    //
    Rectangle {
        color: "#ededed"
        anchors.fill: parent
    }

    //
    // Layout
    //
    ColumnLayout {
        spacing: app.spacing
        anchors.fill: parent
        anchors.margins: app.spacing

        Label {
            font.bold: true
            text: qsTr("Encryption Password") + ":"
        }

        RowLayout {
            Layout.fillWidth: true
            spacing: 2 * app.spacing
            Layout.alignment: Qt.AlignVCenter

            Item {
                width: app.spacing / 2
            }

            Button {
                icon.width: 64
                icon.height: 64
                background: Item{}
                icon.color: "#444444"
                Layout.alignment: Qt.AlignVCenter
                icon.source: "qrc:/icons/password.svg"
            }

            Item {
                width: app.spacing / 2
            }

            ColumnLayout {
                spacing: app.spacing
                Layout.fillWidth: true

                RowLayout {
                    spacing: app.spacing
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignVCenter

                    TextField {
                        id: pwTextfield
                        Layout.fillWidth: true
                        Layout.alignment: Qt.AlignVCenter
                        placeholderText: qsTr("Enter a password") + "..."
                        echoMode: visibleBt.checked ? TextInput.Normal : TextInput.Password
                    }

                    Button {
                        onClicked: pwTextfield.copy()
                        Layout.alignment: Qt.AlignVCenter
                        icon.source: "qrc:/icons/file_copy-24px.svg"
                    }

                    Button {
                        id: visibleBt
                        checkable: true
                        Layout.alignment: Qt.AlignVCenter
                        icon.source: checked ? "qrc:/icons/visibility_off-24px.svg" :
                                               "qrc:/icons/visibility-24px.svg"
                    }
                }

                CheckBox {
                    text: qsTr("Save password (not safe!)")
                }
            }
        }

        Item {
            height: app.spacing
        }

        Label {
            font.bold: true
            text: qsTr("LSB Image Gallery") + ":"
        }

        RowLayout {
            spacing: app.spacing
            Layout.fillWidth: true

            Item {
                width: 2 * app.spacing
            }

            Button {
                icon.width: 64
                icon.height: 64
                background: Item{}
                icon.color: "#444444"
                Layout.alignment: Qt.AlignVCenter
                icon.source: "qrc:/icons/gallery.svg"
            }

            Item {
                width: 2 * app.spacing
            }

            TextField {
                id: searchField
                Layout.minimumWidth: 256
                placeholderText: qsTr("Search") + "..."
                onTextChanged: CBridge.setLsbSearchQuery(text)
            }

            Button {
                icon.width: 24
                icon.height: 24
                background: Item{}
                icon.color: "#444444"
                Layout.alignment: Qt.AlignVCenter
                icon.source: "qrc:/icons/image_search-24px.svg"
            }

            Item {
                Layout.fillWidth: true
            }

            Label {
                text: qsTr("Sort by") + ":"
            }

            ComboBox {
                Layout.minimumWidth: 192
                model: CBridge.sortTypes
                onCurrentIndexChanged: CBridge.changeSortType(currentIndex)
            }
        }

        TextArea {
            readOnly: true
            Layout.fillWidth: true
            Layout.fillHeight: true

            GridView {
                anchors.fill: parent
                anchors.margins: app.spacing
            }
        }

        RowLayout {
            Layout.fillWidth: true

            Button {
                text: qsTr("Open LSB Images Directory")
            }

            Button {
                text: qsTr("Report Issue") + "..."
            }

            Item {
                Layout.fillWidth: true
            }

            Button {
                text: qsTr("Close")
                onClicked: window.close()
            }
        }
    }
}
