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

import Qt.labs.settings 1.0

import "Widgets" as Widgets

ApplicationWindow {
    //
    // Set window name
    //
    id: mw

    //
    // Window geometry
    //
    minimumWidth: 760
    minimumHeight: 520

    //
    // Window title
    //
    title: CAppName + " " + CAppVersion

    //
    // Save/load settings
    //
    Settings {
        property alias mwX: mw.x
        property alias mwY: mw.y
        property alias mwWidth: mw.width
        property alias mwHeight: mw.height
    }

    //
    // Set fusion palette
    //
    palette.base: "#191919"
    palette.text: "#ffffff"
    palette.link: "#2a82da"
    palette.button: "#353535"
    palette.window: "#252525"
    palette.highlight: "#2a82da"
    palette.buttonText: "#ffffff"
    palette.brightText: "#ff0000"
    palette.windowText: "#ffffff"
    palette.toolTipBase: "#ffffff"
    palette.toolTipText: "#ffffff"
    palette.alternateBase: "#353535"
    palette.highlightedText: "#000000"

    //
    // Toolbar
    //
    ToolBar {
        id: toolbar
        height: 48

        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        Rectangle {
            border.width: 1
            border.color: palette.alternateBase

            anchors {
                fill: parent
                topMargin: -border.width
                leftMargin: -border.width * 10
                rightMargin: -border.width * 10
            }

            gradient: Gradient {
                GradientStop { position: 1; color: "#222" }
                GradientStop { position: 0; color: "#444" }
            }
        }

        RowLayout {
            spacing: app.spacing
            anchors.fill: parent
            anchors.margins: app.spacing

            ToolButton {
                icon.width: 24
                icon.height: 24
                text: qsTr("Export")
                Layout.fillHeight: true
                icon.color: palette.text
                onClicked: CBridge.saveImages()
                icon.source: "qrc:/icons/save-24px.svg"
            }

            ToolButton {
                icon.width: 24
                icon.height: 24
                Layout.fillHeight: true
                icon.color: palette.text
                text: qsTr("Select Base Image")
                onClicked: CBridge.selectLsbImageSource()
                icon.source: "qrc:/icons/image_search-24px.svg"
            }

            ToolButton {
                icon.width: 24
                icon.height: 24
                Layout.fillHeight: true
                icon.color: palette.text
                text: qsTr("Decode Image")
                onClicked: CBridge.extractInformation()
                icon.source: "qrc:/icons/unarchive-24px.svg"
            }


            ToolButton {
                icon.width: 24
                icon.height: 24
                checkable: true
                Layout.fillHeight: true
                icon.color: palette.text
                text: checked ? qsTr("Composite Image") :
                                qsTr("Differential Image")
                icon.source: checked ? "qrc:/icons/grain-24px.svg" :
                                       "qrc:/icons/show_chart-24px.svg"
                onCheckedChanged: {
                    if (checked)
                        app.lsbImageUrl = "image://lsb/data"
                    else
                        app.lsbImageUrl = "image://lsb/composite"
                }
            }

            Item {
                Layout.fillWidth: true
            }

            CheckBox {
                id: generateLsbImagesCheck
                text: qsTr("Auto-generate image")
                onCheckedChanged: CBridge.enableGeneratedImages(checked)

                Connections {
                    target: CBridge
                    onLsbImageSourceChanged: generateLsbImagesCheck.checked = CBridge.generateImages
                }
            }
        }
    }

    //
    // MainWindow Layout
    //
    Widgets.ChatRoom {
        anchors.fill: parent
        anchors.margins: app.spacing
        anchors.topMargin: app.spacing + toolbar.height
    }
}
