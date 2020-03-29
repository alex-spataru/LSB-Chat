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

#ifndef P2P_MANAGER_H
#define P2P_MANAGER_H

#include <QTimer>
#include <QObject>
#include <QByteArray>
#include <QUdpSocket>
#include <QHostAddress>

class Discovery;
class NetworkComms;
class P2P_Connection;

class P2P_Manager : public QObject
{
      Q_OBJECT

   public: P2P_Manager(NetworkComms* comms);

      QString userName() const;
      void startBroadcasting();
      void setServerPort(const quint16 port);
      bool isLocalHostAddress(const QHostAddress& address);

   signals:
      void newConnection(P2P_Connection* connection);

   private slots:
      void sendBroadcastDatagram();
      void readBroadcastDatagram();

   private:
      void updateAddresses();

   private:
      quint16 m_serverPort;
      QString m_username;
      NetworkComms* m_client;

      QList<QHostAddress> m_broadcastAddresses;
      QList<QHostAddress> m_ipAddresses;

      QTimer m_broadcastTimer;
      QUdpSocket m_broadcastSocket;
};

#endif
