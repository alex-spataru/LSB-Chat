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

#include <QFont>
#include <QObject>
#include <QElapsedTimer>
#include <QQuickImageProvider>

#include "LSB/LSB.h"
#include "Comms/NetworkComms.h"

class QmlBridge : public QObject
{
    Q_OBJECT Q_PROPERTY(QString userName READ getUserName CONSTANT)
    Q_PROPERTY(QStringList peers READ getPeers NOTIFY peerCountChanged)
    Q_PROPERTY(QString password READ getPassword WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(bool cryptoEnabled READ getCryptoEnabled WRITE setCryptoEnabled NOTIFY
               cryptoEnabledChanged)
    Q_PROPERTY(bool generateImages READ getGenerateImagesEnabled WRITE enableGeneratedImages NOTIFY
               lsbImageSourceChanged)

signals:
    void lsbImageChanged();
    void chatTextUpdated();
    void passwordChanged();
    void peerCountChanged();
    void cryptoEnabledChanged();
    void lsbImageSourceChanged();
    void newParticipant(const QString& name);
    void participantLeft(const QString& name);
    void newMessage(const QString& user, const QString& message, bool encrypted);

public:
    QmlBridge();

    QImage userImage() const;
    QString getUserName() const;
    QString getPassword() const;
    QStringList getPeers() const;
    bool getCryptoEnabled() const;
    bool getGenerateImagesEnabled() const;

public slots:
    void init();
    void sendFile();
    void saveImages();
    void extractInformation();
    void selectLsbImageSource();
    void sendMessage(const QString& text);
    void setPassword(const QString& password);
    void setCryptoEnabled(const bool enabled);
    void enableGeneratedImages(const bool enabled);

private slots:
    void handleNewParticipant(const QString& name);
    void handleParticipantLeft(const QString& name);
    void handleMessages(const QString& name, const QByteArray& data);

private:
    QString saveFile(const QString& name, const QByteArray& data, bool* ok);
    QByteArray encryptData(const QByteArray& data, bool* ok, bool* continueSending);

private:
    QImage m_userImage;
    QString m_password;
    QStringList m_peers;
    bool m_cryptoEnabled;
    NetworkComms m_comms;
    QElapsedTimer m_elapsedTimer;
    QStringList m_availableImages;
};

class LsbImageProvider : public QQuickImageProvider
{
public:
    LsbImageProvider() : QQuickImageProvider(QQuickImageProvider::Pixmap)
    {
        // Nothing to do
    }

    QPixmap requestPixmap(const QString& id, QSize* size, const QSize& requestedSize) override
    {
        Q_UNUSED(size)

        // Show composite image
        if(id == "composite")
            return QPixmap::fromImage(LSB::currentCompositeImage()).scaled(requestedSize);

        // Show LSB data image
        else if(id == "data")
            return QPixmap::fromImage(LSB::currentImageData()).scaled(requestedSize);

        // Generate empty pixmap
        QPixmap pixmap(requestedSize);
        return pixmap;
    }
};
