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
    minimumWidth: 900
    minimumHeight: 740

    //
    // Window title
    //
    title: CAppName + " v" + CAppVersion

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
    // MainWindow Layout
    //
    ColumnLayout {
        spacing: app.spacing

        anchors {
            fill: parent
            margins: app.spacing
        }

        Widgets.ChatRoom {
            Layout.fillWidth: true
            Layout.fillHeight: true
        }

        Widgets.ChatTextBox {
            Layout.fillWidth: true
        }
    }
}
