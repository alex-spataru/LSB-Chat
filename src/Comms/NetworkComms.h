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

#ifndef NETWORK_COMMS_H
#define NETWORK_COMMS_H

#include <QHash>
#include <QHostAddress>
#include <QAbstractSocket>

#include "TCP_Listener.h"

class P2P_Manager;
class NetworkComms : public QObject
{
      Q_OBJECT

   signals:
      void newParticipant(const QString& username);
      void participantLeft(const QString& username);
      void newMessage(const QString& from, const QByteArray& data);

   public:
      NetworkComms();

      QString username() const;
      void sendBinaryData(const QByteArray& data);
      bool hasConnection(const QHostAddress& senderIp, int senderPort = -1) const;

   private slots:
      void readyForUse();
      void disconnected();
      void newConnection(P2P_Connection* connection);
      void removeConnection(P2P_Connection* connection);
      void connectionError(QAbstractSocket::SocketError error);

   private:
      P2P_Manager* m_manager;
      TCP_Listener m_listener;
      QMultiHash<QHostAddress, P2P_Connection*> m_peers;
};

#endif
