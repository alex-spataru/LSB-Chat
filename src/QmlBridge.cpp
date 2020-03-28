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

#include "QmlBridge.h"

QmlBridge::QmlBridge() {
    connect(&m_comms, SIGNAL(newParticipant(QString)),
            this,     SIGNAL(newParticipant(QString)));
    connect(&m_comms, SIGNAL(participantLeft(QString)),
            this,     SIGNAL(participantLeft(QString)));
    connect(&m_comms, SIGNAL(newMessage(QString, QByteArray)),
            this,       SLOT(handleMessages(QString, QByteArray)));
    connect(this,     SIGNAL(newParticipant(QString)),
            this,       SLOT(handleNewParticipant(QString)));
    connect(this,     SIGNAL(participantLeft(QString)),
            this,       SLOT(handleParticipantLeft(QString)));
}

QStringList QmlBridge::getPeers() const {
    return m_peers;
}

QStringList QmlBridge::getLsbImagePaths() const {
    return m_lsbImagePaths;
}

QStringList QmlBridge::getLsbImageDates() const {
    return m_lsbImageDates;
}

QStringList QmlBridge::getLsbImageAuthors() const {
    return m_lsbImageAuthors;
}

QString QmlBridge::getUserName() const {
    return m_comms.username();
}

QString QmlBridge::getLsbImagesDirectory() const {
    return "";
}

void QmlBridge::init() {
    newParticipant(getUserName());
}

void QmlBridge::sendFile() {

}


void QmlBridge::updateLsbImageDb() {

}

void QmlBridge::openLsbImagesDirectory() {

}

void QmlBridge::sendMessage(const QString& text) {
    if (text.isEmpty())
        return;

    emit newMessage(getUserName(), text);
}

void QmlBridge::setPassword(const QString& password) {
}

void QmlBridge::openMessage(const QString& filePath, const QString& password) {
    if (filePath.isEmpty())
        return;
}

void QmlBridge::handleNewParticipant(const QString& name) {
    if (!getPeers().contains(name)) {
        m_peers.append(name);
        emit peerCountChanged();
    }
}

void QmlBridge::handleParticipantLeft(const QString& name) {
    if (getPeers().contains(name)) {
        m_peers.removeAt(m_peers.indexOf(name));
        emit peerCountChanged();
    }
}

void QmlBridge::handleMessages(const QString& name, const QByteArray& data) {
    if (data.isEmpty())
        return;
}
