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

#include "P2P_Manager.h"
#include "NetworkComms.h"
#include "P2P_Connection.h"

/**
 * @brief NetworkComms::NetworkComms
 *
 * Initializes the peer manager and the TCP listener
 */
NetworkComms::NetworkComms()
{
    // Create peer manager
    m_manager = new P2P_Manager (this);
    m_manager->setServerPort (m_listener.serverPort());
    m_manager->startBroadcasting();

    // Connect signals/slots
    connect (m_manager,   SIGNAL (newConnection (P2P_Connection*)),
             this,          SLOT (newConnection (P2P_Connection*)));
    connect (&m_listener, SIGNAL (newConnection (P2P_Connection*)),
             this,          SLOT (newConnection (P2P_Connection*)));
}

/**
 * @brief NetworkComms::username
 * @return
 *
 * Returns the full user name of the local client
 */
QString NetworkComms::username() const
{
    return m_manager->userName() + "@" + QHostInfo::localHostName();
}

/**
 * @brief NetworkComms::sendBinaryData
 * @param data
 *
 * Sends the given @a data to all connected peers
 */
void NetworkComms::sendBinaryData (const QByteArray& data)
{
    // Data is empty, abort
    if (data.isEmpty())
        return;

    // Send data to each connected peer
    foreach (P2P_Connection* connection, m_peers.values())
        connection->sendBinaryData (data);
}

/**
 * @brief NetworkComms::hasConnection
 * @param senderIp
 * @param senderPort
 * @return
 *
 * Returns @c true if the @a senderIp and the @a senderPort are already registered with
 * the peer list. This function is used to avoid generating duplicated connections between
 * two peers on the network.
 */
bool NetworkComms::hasConnection (const QHostAddress& senderIp, int senderPort) const
{
    // We don't know the port, just check the IP address
    if (senderPort == -1)
        return m_peers.contains (senderIp);

    // IP address not found
    if (!m_peers.contains (senderIp))
        return false;

    // Check connection port of every peer
    foreach (P2P_Connection* connection, m_peers.values())
        if (connection->peerPort() == senderPort)
            return true;

    // Should not happen
    return false;
}

/**
 * @brief NetworkComms::readyForUse
 *
 * Finalizes the setup of a new connection handler.
 */
void NetworkComms::readyForUse()
{
    // Get pointer to sender
    P2P_Connection* c = qobject_cast<P2P_Connection*> (sender());

    // Invalid type conversion
    if (!c)
        return;

    // Check that the connection is not already registered
    if (hasConnection (c->peerAddress(), c->peerPort()))
        return;

    // Connect signals/slots
    connect (c,    SIGNAL (newMessage (QString, QByteArray)),
             this, SIGNAL (newMessage (QString, QByteArray)));

    // Register new connection to peer list
    m_peers.insert (c->peerAddress(), c);

    // Get user name and notify app
    QString user = c->name();
    if (!user.isEmpty())
        emit newParticipant (user);
}

/**
 * @brief NetworkComms::disconnected
 * Removes the signaling connection handler from the peer list
 */
void NetworkComms::disconnected()
{
    // Remove the connection manager that emitted the signal
    if (P2P_Connection* c = qobject_cast<P2P_Connection*> (sender()))
        removeConnection (c);
}

/**
 * @brief NetworkComms::newConnection
 * @param connection
 *
 * Configures the connection and allows it to start communicating with the peer
 */
void NetworkComms::newConnection (P2P_Connection* connection)
{
    // Check pointer
    Q_ASSERT (connection);

    // Set greeting message with local user name
    connection->setGreetingMessage (m_manager->userName());

    // Connect signals/slots
    connect (connection, SIGNAL (error (QAbstractSocket::SocketError)),
             this,         SLOT (connectionError (QAbstractSocket::SocketError)));
    connect (connection, SIGNAL (disconnected()),
             this,         SLOT (disconnected()));
    connect (connection, SIGNAL (readyForUse()),
             this,         SLOT (readyForUse()));
}

/**
 * @brief NetworkComms::removeConnection
 * @param connection
 *
 * Removes the given @a connection from the peer list and deletes it from memory
 */
void NetworkComms::removeConnection (P2P_Connection* connection)
{
    // Check pointer
    Q_ASSERT (connection);

    // Remove the connection from the peer list
    if (m_peers.contains (connection->peerAddress())) {
        // Remove from peer list
        m_peers.remove (connection->peerAddress());

        // Get username and notify app about user leaving chat room
        QString user = connection->name();
        if (!user.isEmpty())
            emit participantLeft (user);
    }

    // Delete conenction handler
    connection->deleteLater();
}

/**
 * @brief NetworkComms::connectionError
 * @param error
 *
 * Removes the signaling connection handler from the peer list when a socket error ocurrs.
 */
void NetworkComms::connectionError (QAbstractSocket::SocketError error)
{
    // We don't use the argument...
    Q_UNUSED (error)

    // Remove the connection manager that emitted the signal
    if (P2P_Connection* c = qobject_cast<P2P_Connection*> (sender()))
        removeConnection (c);
}
