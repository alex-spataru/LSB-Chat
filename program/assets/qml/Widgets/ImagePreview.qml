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

GroupBox {
    id: controls

    //
    // Configure the size of the image
    //
    property int imageSize: 196

    //
    // Set background rectangle
    //
    background: Rectangle {
        border.width: 1
        color: palette.base
        anchors.fill: parent
        border.color: palette.alternateBase
        anchors.topMargin: title.length ? 20 : 0
    }

    //
    // Layout
    //
    ColumnLayout {
        anchors.fill: parent

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
                anchors.fill: parent
                source: app.lsbImageUrl
                sourceSize: Qt.size(imageSize, imageSize)
                fillMode: Image.PreserveAspectCrop

                Connections {
                    target: CBridge
                    onLsbImageChanged: {
                        image.source = ""
                        image.source = app.lsbImageUrl
                    }
                }

                Connections {
                    target: app
                    onLsbImageUrlChanged: {
                        image.source = ""
                        image.source = app.lsbImageUrl
                    }
                }
            }
        }
    }
}
