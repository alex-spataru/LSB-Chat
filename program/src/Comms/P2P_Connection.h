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

#ifndef P2P_CONNECTION_H
#define P2P_CONNECTION_H

#include <QTimer>
#include <QtNetwork>
#include <QTcpSocket>
#include <QTimerEvent>
#include <QHostAddress>
#include <QElapsedTimer>

class P2P_Connection : public QTcpSocket
{
    Q_OBJECT

signals:
    void readyForUse();
    void newMessage(const QString& from, const QByteArray& message);

public:
    enum DataType {
        BinaryData,
        Ping,
        Pong,
        Greeting,
        Undefined
    };

    P2P_Connection(QObject* parent = Q_NULLPTR);
    P2P_Connection(qintptr socketDescriptor, QObject* parent = Q_NULLPTR);
    ~P2P_Connection() override;

    QString name();
    void setGreetingMessage(const QString& message);
    bool sendBinaryData(const QByteArray& data);

private slots:
    void sendPing();
    void sendPong();
    void processReadyRead();
    void sendGreetingMessage();

private:
    bool sendData(const QByteArray& data);
    void readPacket(QByteArray& packet);
    void processGreeting(QByteArray& data);

    QByteArray headerEndCode() const;
    QByteArray headerStartCode() const;

private:
    QString m_username;
    QTimer m_pingTimer;
    QByteArray m_buffer;
    QString m_greetingMessage;
    QElapsedTimer m_pongTimer;
    bool m_greetingMessageSent;
};

#endif
