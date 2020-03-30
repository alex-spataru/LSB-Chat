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
    Settings {
        property alias pw: pwTextField.text
    }

    background: Rectangle {
        border.width: 1
        color: palette.base
        anchors.fill: parent
        border.color: palette.alternateBase
        anchors.topMargin: title.length ? 20 : 0
    }

    RowLayout {
        anchors.fill: parent
        spacing: app.spacing

        TextField {
            id: pwTextField
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignVCenter
            placeholderText: qsTr("Set an encryption key") + "..."
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
}

