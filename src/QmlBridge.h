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

#include <QObject>

#include "Comms/NetworkComms.h"

class QmlBridge : public QObject
{
    Q_OBJECT
    Q_PROPERTY (QString userName READ getUserName CONSTANT)
    Q_PROPERTY (QStringList peers READ getPeers NOTIFY peerCountChanged)
    Q_PROPERTY (QStringList lsbImagePaths READ getLsbImagePaths NOTIFY lsbDbUpdated)
    Q_PROPERTY (QStringList lsbImageDates READ getLsbImageDates NOTIFY lsbDbUpdated)
    Q_PROPERTY (QStringList lsbImageAuthorrs READ getLsbImageAuthors NOTIFY lsbDbUpdated)

signals:
    void lsbDbUpdated();
    void chatTextUpdated();
    void peerCountChanged();
    void newParticipant (const QString& name);
    void participantLeft (const QString& name);
    void newMessage (const QString& user, const QString& message);

public:
    QmlBridge();

    QStringList getPeers() const;
    QStringList getLsbImagePaths() const;
    QStringList getLsbImageDates() const;
    QStringList getLsbImageAuthors() const;

    QString getUserName() const;
    QString getLsbImagesDirectory() const;

public slots:
    void init();
    void sendFile();
    void updateLsbImageDb();
    void openLsbImagesDirectory();
    void sendMessage (const QString& text);
    void setPassword (const QString& password);
    void openMessage (const QString& filePath, const QString& password);

private slots:
    void handleNewParticipant (const QString& name);
    void handleParticipantLeft (const QString& name);
    void handleMessages (const QString& name, const QByteArray& data);

private:
    QStringList m_peers;
    QStringList m_lsbImagePaths;
    QStringList m_lsbImageDates;
    QStringList m_lsbImageAuthors;

    NetworkComms m_comms;
};
