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

#include "P2P_Connection.h"

/*
 * Define comm. protocol time intervals
 */
static const int PONG_TIMEOUT = 60 * 1000;
static const int PING_INTERVAL = 5 * 1000;

/**
 * @brief P2P_Connection::P2P_Connection
 * @param parent
 *
 * Initializes the CBOR writter, configures the signals/slots and initializes all internal
 * variables of the class.
 */
P2P_Connection::P2P_Connection(QObject* parent) : QTcpSocket(parent)
{
    // Initialize internal variables
    m_username = "Unknown";
    m_greetingMessageSent = false;
    m_greetingMessage = "Undefined";

    // Set ping timer interval
    m_pingTimer.setInterval(PING_INTERVAL);

    // Configure signals/slots
    QObject::connect(this,         SIGNAL(readyRead()),
                     this,         SLOT(processReadyRead()));
    QObject::connect(this,         SIGNAL(disconnected()),
                     &m_pingTimer, SLOT(stop()));
    QObject::connect(&m_pingTimer, SIGNAL(timeout()),
                     this,         SLOT(sendPing()));
    QObject::connect(this,         SIGNAL(connected()),
                     this,         SLOT(sendGreetingMessage()));
}

P2P_Connection::P2P_Connection(qintptr socketDescriptor,
                               QObject* parent) : P2P_Connection(parent)
{
    setSocketDescriptor(socketDescriptor);
}

/**
 * @brief P2P_Connection::~P2P_Connection
 *
 * Wait a little to indicate a clean/safe shutdown of the socket before destroying it
 */
P2P_Connection::~P2P_Connection()
{
    if(m_greetingMessageSent)
        waitForBytesWritten(2000);
}

/**
 * @brief P2P_Connection::name
 * @return
 *
 * Returns the user name of the peer
 */
QString P2P_Connection::name()
{
    return m_username;
}

/**
 * @brief P2P_Connection::setGreetingMessage
 * @param message
 *
 * Changes the greeting message to be sent to the peer. Ideally, the greeting message should
 * be the user name of the local client.
 */
void P2P_Connection::setGreetingMessage(const QString& message)
{
    m_greetingMessage = message;
}

/**
 * @brief P2P_Connection::sendBinaryData
 * @param data
 * @return
 *
 * Sends the given binary @a data to the peer
 */
bool P2P_Connection::sendBinaryData(const QByteArray& data)
{
    // Data is empty, abort
    if(data.isEmpty())
        return false;

    // Send header code & data
    QByteArray packet;
    packet.append(headerStartCode());
    packet.append(BinaryData);
    packet.append(data);
    packet.append(BinaryData);
    packet.append(headerEndCode());

    // Return success state
    return sendData(packet);
}

/**
 * @brief P2P_Connection::processReadyRead
 *
 * Process and react to incoming data from peer
 */
void P2P_Connection::processReadyRead()
{
    // Add data to buffer
    m_buffer.append(readAll());

    // Get range from which
    int initIndex = m_buffer.indexOf(headerStartCode());
    int stopIndex = m_buffer.indexOf(headerEndCode());

    // Packet incomplete
    if (initIndex < 0 || stopIndex < 0)
        return;

    // Increment stop index to last byte of
    stopIndex += headerEndCode().length();

    // Construct byte array with packet from buffer
    QByteArray packet;
    for (int i = initIndex; i < stopIndex; ++i)
        packet.append(m_buffer.at(i));

    // Remove packet from buffer
    m_buffer.remove(0, stopIndex);

    // Chop header codes from packet
    packet.remove(0, headerStartCode().length());
    packet.chop(headerEndCode().length());

    // Analize packet
    if (!packet.isEmpty())
        readPacket(packet);
}

/**
 * @brief P2P_Connection::sendPing
 *
 * Sends a pong message, or cancels the connection if the abort conditions are met
 */
void P2P_Connection::sendPing()
{
    // Peer response time is longer than the timeout, cancel connection
    if(m_pongTimer.elapsed() > PONG_TIMEOUT) {
        abort();
        return;
    }

    // Send header code & data
    QByteArray packet;
    packet.append(headerStartCode());
    packet.append(Ping);
    packet.append(headerEndCode());

    // Send ping
    sendData(packet);
}

/**
 * @brief P2P_Connection::sendPing
 *
 * Sends a pong message. This packet is sent in response to a ping.
 */
void P2P_Connection::sendPong()
{
    // Send header code & data
    QByteArray packet;
    packet.append(headerStartCode());
    packet.append(Pong);
    packet.append(headerEndCode());

    // Send pong respongse
    sendData(packet);
}

/**
 * @brief P2P_Connection::sendGreetingMessage
 *
 * Sends the greeting message and configures the CBOR reader
 */
void P2P_Connection::sendGreetingMessage()
{
    // Send header code & data
    QByteArray packet;
    packet.append(headerStartCode());
    packet.append(Greeting);
    packet.append(m_greetingMessage);
    packet.append(Greeting);
    packet.append(headerEndCode());

    // Only send the greeting message once
    if (sendData(packet))
        m_greetingMessageSent = true;
}

bool P2P_Connection::sendData(const QByteArray& data)
{
    return write(data) == data.length();
}

/**
 * @brief P2P_Connection::readPacket
 *
 * Called when a new message has been completely received, this function processes the message data
 * and reacts according to the message type.
 */
void P2P_Connection::readPacket(QByteArray& packet)
{
    // Get packet type
    char packetStart = packet.at(0);
    char packetEnd = packet.at(packet.length() - 1);

    // Start and end codes do not match, protocol error
    if (packetStart != packetEnd)
        return;

    // Get packet type
    DataType dataType = static_cast<DataType>(packetStart);

    // Chop data type bytes from packet
    packet.remove(0, 1);
    packet.chop(1);

    // React to packet
    switch(dataType) {
    case Greeting:
        processGreeting(packet);
        break;
    case BinaryData:
        emit newMessage(m_username, packet);
        break;
    case Ping:
        sendPong();
        break;
    case Pong:
        m_pongTimer.restart();
        break;
    default:
        break;
    }
}

/**
 * @brief P2P_Connection::processGreeting
 * @param data
 *
 * Processes the given @a data, extracts user information from greeting & begins ping/pong cycle.
 */
void P2P_Connection::processGreeting(QByteArray& data)
{
    // Construct user name
    m_username = data + '@' + QHostAddress(peerAddress().toIPv4Address()).toString();

    // Cancel if connection is invalid
    if (!isValid()) {
        abort();
        return;
    }

    // Start ping/pong cycle
    m_pingTimer.start();
    m_pongTimer.start();

    // Send greeting message
    if (!m_greetingMessageSent)
        sendGreetingMessage();

    emit readyForUse();
}

/**
 * @brief P2P_Connection::headerEndCode
 * @return
 *
 * Returns a byte array with the common packet end code
 */
QByteArray P2P_Connection::headerEndCode() const
{
    QByteArray code;
    code.append(0x01); // SOH
    code.append(0x50); // P
    code.append(0x02); // 2
    code.append(0x50); // P
    code.append(0x5F); // _
    code.append(0x45); // E
    code.append(0x4E); // N
    code.append(0x44); // D
    code.append(0x17); // ETB
    code.append(0x04); // EOT
    return code;
}

/**
 * @brief P2P_Connection::headerEndCode
 * @return
 *
 * Returns a byte array with the common packet header code
 */
QByteArray P2P_Connection::headerStartCode() const
{
    QByteArray code;
    code.append(0x01); // SOH
    code.append(0x50); // P
    code.append(0x02); // 2
    code.append(0x50); // P
    code.append(0x5F); // _
    code.append(0x53); // S
    code.append(0x54); // T
    code.append(0x41); // A
    code.append(0x52); // R
    code.append(0x54); // T
    code.append(0x17);  // ETB
    return code;
}
