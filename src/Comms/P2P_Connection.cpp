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
static const int TRANSFER_TIMEOUT = 30 * 1000;

/*
 * Protocol is defined as follows, using the CBOR Data Definition Language:
 *
 *  protocol    = [
 *     greeting,        ; must start with a greeting command
 *     * command        ; zero or more regular commands after
 *  ]
 *  command       = binary data / ping / pong / greeting
 *  binary data   = { 0 => byte array }
 *  ping          = { 1 => null       }
 *  pong          = { 2 => null       }
 *  greeting      = { 3 => byte array }
 */

/**
 * @brief P2P_Connection::P2P_Connection
 * @param parent
 *
 * Initializes the CBOR writter, configures the signals/slots and initializes all internal
 * variables of the class.
 */
P2P_Connection::P2P_Connection(QObject* parent) : QTcpSocket(parent), m_writer(this) {
    // Initialize internal variables
    m_username = "Unknown";
    m_transferTimerId = -1;
    m_currentDataType = Undefined;
    m_greetingMessageSent = false;
    m_greetingMessage = "Undefined";
    m_connectionState = WaitingForGreeting;

    // Set ping timer interval
    m_pingTimer.setInterval(PING_INTERVAL);

    // Configure signals/slots
    QObject::connect(this,         SIGNAL (readyRead()),
                     this,         SLOT   (processReadyRead()));
    QObject::connect(this,         SIGNAL (disconnected()),
                     &m_pingTimer, SLOT   (stop()));
    QObject::connect(&m_pingTimer, SIGNAL (timeout()),
                     this,         SLOT   (sendPing()));
    QObject::connect(this,         SIGNAL (connected()),
                     this,         SLOT   (sendGreetingMessage()));
}

P2P_Connection::P2P_Connection(qintptr socketDescriptor, QObject* parent) : P2P_Connection(parent) {
    setSocketDescriptor(socketDescriptor);
    m_reader.setDevice(this);
}

/**
 * @brief P2P_Connection::~P2P_Connection
 *
 * Wait a little to indicate a clean/safe shutdown of the socket before destroying it
 */
P2P_Connection::~P2P_Connection() {
    if (m_greetingMessageSent) {
        m_writer.endArray();
        waitForBytesWritten(2000);
    }
}

/**
 * @brief P2P_Connection::name
 * @return
 *
 * Returns the user name of the peer
 */
QString P2P_Connection::name() {
    return m_username;
}

/**
 * @brief P2P_Connection::setGreetingMessage
 * @param message
 *
 * Changes the greeting message to be sent to the peer. Ideally, the greeting message should
 * be the user name of the local client.
 */
void P2P_Connection::setGreetingMessage(const QString& message) {
    m_greetingMessage = message;
}

/**
 * @brief P2P_Connection::sendBinaryData
 * @param data
 * @return
 *
 * Sends the given binary @a data to the peer
 */
bool P2P_Connection::sendBinaryData(const QByteArray& data) {
    // Data is empty, abort
    if (data.isEmpty())
        return false;

    // Send data packet through CBOR stream
    m_writer.startMap(1);           // Start new message
    m_writer.append(BinaryData);    // Set message type
    m_writer.append(data);          // Append data payload
    m_writer.endMap();              // End message

    // Return success state
    return true;
}

/**
 * @brief P2P_Connection::timerEvent
 * @param event
 *
 * If the data transfer timer expires, abort the connection to avoid buffer overload and/or
 * loss of information.
 */
void P2P_Connection::timerEvent(QTimerEvent* event) {
    if (event->timerId() == m_transferTimerId) {
        abort();
        killTimer(m_transferTimerId);
        m_transferTimerId = -1;
    }
}

/**
 * @brief P2P_Connection::processReadyRead
 *
 * Process and react to incoming data from peer
 */
void P2P_Connection::processReadyRead() {
    // we've got more data, let's parse
    m_reader.reparse();
    while (m_reader.lastError() == QCborError::NoError) {
        if (m_connectionState == WaitingForGreeting) {
            if (!m_reader.isArray())
                break;                  // protocol error

            m_reader.enterContainer();    // we'll be in this array forever
            m_connectionState = ReadingGreeting;
        } else if (m_reader.containerDepth() == 1) {
            // Current m_connectionState: no command read
            // Next m_connectionState: read command ID
            if (!m_reader.hasNext()) {
                m_reader.leaveContainer();
                disconnectFromHost();
                return;
            }

            if (!m_reader.isMap() || !m_reader.isLengthKnown() || m_reader.length() != 1)
                break;                  // protocol error
            m_reader.enterContainer();
        } else if (m_currentDataType == Undefined) {
            // Current m_connectionState: read command ID
            // Next m_connectionState: read command payload
            if (!m_reader.isInteger())
                break;                  // protocol error
            m_currentDataType = DataType(m_reader.toInteger());
            m_reader.next();
        } else {
            // Current m_connectionState: read command payload
            if (m_reader.isByteArray()) {
                auto r = m_reader.readByteArray();
                m_buffer += r.data;
                if (r.status != QCborStreamReader::EndOfString)
                    continue;
            } else if (m_reader.isNull()) {
                m_reader.next();
            } else {
                break;                   // protocol error
            }

            // Next m_connectionState: no command read
            m_reader.leaveContainer();
            if (m_transferTimerId != -1) {
                killTimer(m_transferTimerId);
                m_transferTimerId = -1;
            }

            if (m_connectionState == ReadingGreeting) {
                if (m_currentDataType != Greeting)
                    break;              // protocol error
                processGreeting();
            } else {
                processData();
            }
        }
    }

    if (m_reader.lastError() != QCborError::EndOfFile)
        abort();       // parse error

    if (m_transferTimerId != -1 && m_reader.containerDepth() > 1)
        m_transferTimerId = startTimer(TRANSFER_TIMEOUT);
}

/**
 * @brief P2P_Connection::sendPing
 *
 * Sends a pings message, or cancels the connection if the abort conditions are met
 */
void P2P_Connection::sendPing() {
    // Peer response time is longer than the timeout, cancel connection
    if (m_pongTimer.elapsed() > PONG_TIMEOUT) {
        abort();
        return;
    }

    // Send ping packet through CBOR stream
    m_writer.startMap(1);       // Start new message
    m_writer.append(Ping);      // Set message type
    m_writer.append(nullptr);   // NULL data payload
    m_writer.endMap();          // End message
}

/**
 * @brief P2P_Connection::sendGreetingMessage
 *
 * Sends the greeting message and configures the CBOR reader
 */
void P2P_Connection::sendGreetingMessage() {
    // Start new CBOR array
    m_writer.startArray();

    // Send greeting packet through CBOR stream
    m_writer.startMap(1);                        // Start new message
    m_writer.append(Greeting);                   // Set message type
    m_writer.append(m_greetingMessage.toUtf8()); // Append greeting message
    m_writer.endMap();                           // End message

    // Only send the greeting message once
    m_greetingMessageSent = true;

    // Configure the CBOR reader if it is not already configured
    if (!m_reader.device())
        m_reader.setDevice(this);
}

/**
 * @brief P2P_Connection::processData
 *
 * Called when a new message has been completely received, this function processes the message data
 * and reacts according to the message type.
 */
void P2P_Connection::processData() {

    // Binary data message received, emit new signal and let the program process the data
    if (m_currentDataType == BinaryData)
        emit newMessage(m_username, m_buffer);

    // Respond to ping packet with a pong packet
    else if (m_currentDataType == Ping) {
        m_writer.startMap(1);
        m_writer.append(Pong);
        m_writer.append(nullptr);
        m_writer.endMap();
    }

    // If we receive pong packet, we know that the peer is still there, reset watchdog timer
    else if (m_currentDataType == Pong)
        m_pongTimer.restart();

    // Clear current data type flag and clear buffer
    m_currentDataType = Undefined;
    m_buffer.clear();
}

/**
 * @brief P2P_Connection::processGreeting
 *
 * Processes a greeting message, responds to the greeting message (if needed), begins the
 * ping/pong loop and completes the peer connection setup process.
 */
void P2P_Connection::processGreeting() {
    // Construct user name from buffer and peer's IPv4 address
    QString ipv4 = QHostAddress(peerAddress().toIPv4Address()).toString();
    m_username = QString::fromUtf8(m_buffer) + "@" + ipv4;

    // Reset current data type and buffer
    m_currentDataType = Undefined;
    m_buffer.clear();

    // Abort connection if it's not valid
    if (!isValid()) {
        abort();
        return;
    }

    // Send greeting message if necessary
    if (!m_greetingMessageSent)
        sendGreetingMessage();

    // Start the ping/pong process
    m_pingTimer.start();
    m_pongTimer.start();

    // Change the connection state
    m_connectionState = ReadyForUse;

    // Notify the rest of the program
    emit readyForUse();
}
