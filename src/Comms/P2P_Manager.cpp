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

/*
 * Define broadcast interval & broadcast port
 */
static const quint16 BROADCAST_PORT = 45000;
static const quint32 BROADCAST_INTERVAL = 2000;

/**
 * @brief P2P_Manager::P2P_Manager
 * @param comms
 *
 * Gets user name from the operating system and starts the UDP broadcast loop
 */
P2P_Manager::P2P_Manager(NetworkComms* comms) : QObject(comms) {
    // Assign client pointer
    m_client = comms;

    // Create list of environmental variables
    static const char *envVariables[] = {
        "USERNAME", "USER", "USERDOMAIN", "HOSTNAME", "DOMAINNAME"
    };

    // Try to get user name from env. variables
    for (const char *varname : envVariables) {
        m_username = qEnvironmentVariable(varname);
        if (!m_username.isEmpty())
            break;
    }

    // If the user name is still unknown, assign fall-back user name
    if (m_username.isEmpty())
        m_username = "unknown";

    // Set server port & update address configuration
    setServerPort(0);
    updateAddresses();

    // Configure socket for UDP broadcast
    m_broadcastSocket.bind(QHostAddress::Any,
                           BROADCAST_PORT,
                           QUdpSocket::ShareAddress |
                           QUdpSocket::ReuseAddressHint);

    // Configure signals/slots
    connect(&m_broadcastSocket, SIGNAL(readyRead()),
            this,                 SLOT(readBroadcastDatagram()));
    connect(&m_broadcastTimer,  SIGNAL(timeout()),
            this,                 SLOT(sendBroadcastDatagram()));

    // Start broadcasting loop
    m_broadcastTimer.setInterval(BROADCAST_INTERVAL);
}

/**
 * @brief P2P_Manager::userName
 * @return
 *
 * Returns user name obtained from the operating system
 */
QString P2P_Manager::userName() const {
    return m_username;
}

/**
 * @brief P2P_Manager::startBroadcasting
 *
 * Starts the UDP broadcast timer, which allows the program to send a periodic broadcast
 * over all available network interfaces.
 */
void P2P_Manager::startBroadcasting() {
    m_broadcastTimer.start();
}

/**
 * @brief P2P_Manager::setServerPort
 * @param port
 *
 * Sets the TCP listener port, which is sent with the UDP datagrams so that other instances
 * of the application in the LAN can know in which port shall they try to connect with the
 * local computer.
 */
void P2P_Manager::setServerPort(const quint16 port) {
    m_serverPort = port;
}

/**
 * @brief P2P_Manager::isLocalHostAddress
 * @param address
 * @return
 *
 * Returns @c true if the given @a address is equal to the address of the client computer
 */
bool P2P_Manager::isLocalHostAddress(const QHostAddress& address) {
    foreach (QHostAddress localAddress, m_ipAddresses) {
        if (address.isEqual(localAddress))
            return true;
    }

    return false;
}

/**
 * @brief P2P_Manager::sendBroadcastDatagram
 *
 * Writes an UDP broadcast packet with the user name and TCP server port over all available
 * network interfaces.
 */
void P2P_Manager::sendBroadcastDatagram() {
    // Create UDP datagram
    QByteArray datagram;
    {
        QCborStreamWriter writer(&datagram);
        writer.startArray(2);
        writer.append(m_username);
        writer.append(m_serverPort);
        writer.endArray();
    }

    // Send UDP packet to all possible broadcast adresses
    bool validBroadcastAddresses = true;
    foreach (QHostAddress address, m_broadcastAddresses) {
        if (m_broadcastSocket.writeDatagram(datagram, address, BROADCAST_PORT) == -1)
            validBroadcastAddresses = false;
    }

    // If there was an error, update the broadcast address list
    if (!validBroadcastAddresses)
        updateAddresses();
}

/**
 * @brief P2P_Manager::readBroadcastDatagram
 *
 * Reads and processes all incoming UDP broadcast datagrams. If the datagram comes from
 * another computer and is valid, a new TCP connection is established with that computer.
 */
void P2P_Manager::readBroadcastDatagram() {
    // Attend each individual datagram
    while (m_broadcastSocket.hasPendingDatagrams()) {
        // Init. variables
        quint16 senderPort;
        QByteArray datagram;
        QHostAddress senderIp;
        qint64 senderServerPort;

        // Resize byte array to fit incoming data size
        datagram.resize(m_broadcastSocket.pendingDatagramSize());

        // Try to read data
        qint64 res = m_broadcastSocket.readDatagram(datagram.data(),
                                                    datagram.size(),
                                                    &senderIp,
                                                    &senderPort);

        // Data reading error
        if (res == -1)
            continue;

        // decode the datagram
        QCborStreamReader reader(datagram);
        if (reader.lastError() != QCborError::NoError || !reader.isArray())
            continue;
        if (!reader.isLengthKnown() || reader.length() != 2)
            continue;

        reader.enterContainer();
        if (reader.lastError() != QCborError::NoError || !reader.isString())
            continue;
        while (reader.readString().status == QCborStreamReader::Ok) {
            // we don't actually need the username right now
        }

        // Datagram error
        if (reader.lastError() != QCborError::NoError || !reader.isUnsignedInteger())
            continue;

        // Get server port
        senderServerPort = reader.toInteger();

        // Do not autoconnect to ourselves
        if (isLocalHostAddress(senderIp) && senderServerPort == m_serverPort)
            continue;

        // Register new connection with client
        if (!m_client->hasConnection(senderIp)) {
            // Create new connection
            P2P_Connection* connection = new P2P_Connection(this);
            emit newConnection(connection);

            // Connect the connection to the target host
            connection->connectToHost(senderIp, senderServerPort);
        }
    }
}

/**
 * @brief P2P_Manager::updateAddresses
 *
 * Updates the broadcast address list to match the current hardware and network configuration
 */
void P2P_Manager::updateAddresses() {
    // Clear address list
    m_ipAddresses.clear();
    m_broadcastAddresses.clear();

    // Send data through all network interfaces
    foreach (QNetworkInterface interface,
             QNetworkInterface::allInterfaces()) {
        // Send data through all address entries
        foreach (QNetworkAddressEntry entry,
                 interface.addressEntries()) {
            // Get entry address
            QHostAddress broadcastAddress = entry.broadcast();

            // Register address if its valid and is not the local host
            if (broadcastAddress != QHostAddress::Null &&
                    entry.ip() != QHostAddress::LocalHost) {
                m_broadcastAddresses << broadcastAddress;
                m_ipAddresses << entry.ip();
            }
        }
    }
}
