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
import QtQuick.Controls 2.0
import Qt.labs.settings 1.0

GroupBox {
    id: controls

    //
    // Set the preview URL to use
    //
    property string lsbImageUrl: "image://lsb/composite"


    //
    // Configure the size of the image
    //
    property int imageSize: 196

    //
    // Save/load settings
    //
    Settings {
        property alias password: passwordTf.text
        property alias previewMode: lsbPreviewModeBt.checked
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
    // Layout
    //
    ColumnLayout {
        anchors.fill: parent

        //
        // Buttons
        //
        GridLayout {
            id: gridLayout
            columns: 4
            rowSpacing: 0
            Layout.fillWidth: true
            readonly property real itemWidth: (controls.width - (columns * app.spacing)) / columns

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
        // Checkbox
        //
        CheckBox {
            id: generateLsbImagesCheck
            text: qsTr("Generate LSB images automatically")
            onCheckedChanged: CBridge.enableGeneratedImages(checked)

            Connections {
                target: CBridge
                onLsbImageSourceChanged: generateLsbImagesCheck.checked = CBridge.generateImages
            }
        }

        //
        // Spacer
        //
        Item {
            Layout.fillHeight: true
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
                placeholderText: qsTr("Enter a password") + "..."
                echoMode: visibleBt.checked ? TextInput.Normal : TextInput.Password

                onTextChanged: {
                    CBridge.setPassword(text)
                    if (text.length > 0)
                        CBridge.setCryptoEnabled(true)
                    else
                        CBridge.setCryptoEnabled(false)
                }

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
            height: Layout.fillHeighttrue
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
            Layout.minimumWidth: imageSize - 2 * app.spacing
            Layout.maximumWidth: imageSize - 2 * app.spacing
            Layout.maximumHeight: imageSize - 2 * app.spacing
            Layout.minimumHeight: imageSize - 2 * app.spacing

            Image {
                id: image
                cache: false
                smooth: false
                anchors.margins: 2
                asynchronous: false
                source: lsbImageUrl
                anchors.fill: parent
                sourceSize: Qt.size(imageSize, imageSize)
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
