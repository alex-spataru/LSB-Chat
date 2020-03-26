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

#include "TCP_Listener.h"
#include "P2P_Connection.h"

/**
 * @brief TCP_Listener::TCP_Listener
 * @param parent
 *
 * Configure the TCP server to listen for connections from any address
 */
TCP_Listener::TCP_Listener(QObject *parent) : QTcpServer(parent) {
    listen(QHostAddress::Any);
}

/**
 * @brief TCP_Listener::incomingConnection
 * @param socketDescriptor
 *
 * Respond to a connection request by establishing a new TCP connection with the petitioner
 */
void TCP_Listener::incomingConnection(qintptr socketDescriptor) {
    P2P_Connection* connection = new P2P_Connection(socketDescriptor, this);
    emit newConnection(connection);
}
