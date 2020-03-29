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

ApplicationWindow {
    //
    // Set window name
    //
    id: mw

    //
    // Window geometry
    //
    minimumWidth: 800
    minimumHeight: 640

    //
    // Window title
    //
    title: CAppName + " v" + CAppVersion

    //
    // Background rectangle
    //
    background: Rectangle {
        color: "#f4f4f4"
    }

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
