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
    property int sidebarSize: 250

    //
    // Chat log
    //
    ColumnLayout {
        spacing: app.spacing
        Layout.fillWidth: true
        Layout.fillHeight: true

        ChatLog {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        ChatTextBox {
            Layout.fillWidth: true
        }
    }

    //
    // Peer list and controls
    //
    ColumnLayout {
        spacing: app.spacing
        Layout.fillWidth: false
        Layout.fillHeight: true

        Controls {
            Layout.fillWidth: true
        }

        PeerList {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        ImagePreview {
            width: 296
            imageSize: width
            Layout.fillHeight: false
            Layout.maximumWidth: 296
        }
    }
}
