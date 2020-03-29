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

RowLayout {
    spacing: app.spacing

    //
    // Define signals
    //
    signal attachClicked()
    signal settingsClicked()
    signal messageSent(var message)
    signal cryptoClicked(var enabled)

    //
    // Clear message text field when a message is sent
    //
    onMessageSent: textField.clear()

    //
    // Settings button
    //
    Button {
        onClicked: settingsClicked()
        Layout.alignment: Qt.AlignVCenter
        icon.source: "qrc:/icons/settings-24px.svg"
    }

    //
    // Crypto button
    //
    Button {
        Layout.alignment: Qt.AlignVCenter
        onClicked: cryptoClicked(!CBridge.cryptoEnabled)
        icon.source: CBridge.cryptoEnabled ? "qrc:/icons/enhanced_encryption-24px.svg" :
                                             "qrc:/icons/no_encryption-24px.svg"
    }

    //
    // Message field
    //
    TextField {
        id: textField
        Layout.fillWidth: true
        Layout.alignment: Qt.AlignVCenter
        placeholderText: qsTr("Please type a message") + "..."
        onAccepted: messageSent(text)
    }

    //
    // Send button
    //
    Button {
        Layout.alignment: Qt.AlignVCenter
        enabled: textField.text.length > 0
        onClicked: messageSent(textField.text)
        icon.source: "qrc:/icons/send-24px.svg"
    }

    //
    // Attach button
    //
    Button {
        onClicked: attachClicked()
        Layout.alignment: Qt.AlignVCenter
        icon.source: "qrc:/icons/attach_file-24px.svg"
    }
}
