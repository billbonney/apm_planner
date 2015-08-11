/*=====================================================================

QGroundControl Open Source Ground Control Station

(c) 2009 - 2011 QGROUNDCONTROL PROJECT <http://www.qgroundcontrol.org>

This file is part of the QGROUNDCONTROL project

    QGROUNDCONTROL is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    QGROUNDCONTROL is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with QGROUNDCONTROL. If not, see <http://www.gnu.org/licenses/>.

======================================================================*/

/**
 * @file
 *   @brief Definition of UDP connection (server) for unmanned vehicles
 *   @author Lorenz Meier <mavteam@student.ethz.ch>
 *
 */
#include <QtGlobal>

#include <QTimer>
#include <QList>
#include <QMutexLocker>
#include <iostream>
#include <QHostInfo>

#include "QsLog.h"
#include "UDPLink.h"
#include "LinkManager.h"
#include "QGC.h"


UDPLink::UDPLink(QHostAddress host, quint16 port) :
    _sourceHost(host),
    _port(port),
    _packetsReceived(false)
{
    // Set unique ID and add link to the list of links
    _linkId = getNextLinkId();
    setName(tr("UDP (%1:%2)").arg(_sourceHost.toString()).arg(_port));
    QLOG_INFO() << "Listening on " << _name;

    QObject::connect(&_socket, SIGNAL(readyRead()), this, SLOT(readBytes()));

    QObject::connect(&_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(_socketError(QAbstractSocket::SocketError)));
    QObject::connect(&_socket, SIGNAL(disconnected()), this, SLOT(_socketDisconnected()));
}

UDPLink::~UDPLink()
{
    disconnect();
    this->deleteLater();
}

/**
 * @brief Runs the thread
 *
 **/
void UDPLink::run()
{
    exec();
}

void UDPLink::setAddress(QHostAddress host)
{
    bool reconnect = false;
    if(isConnected())
    {
        disconnect();
        reconnect = true;
    }
    _sourceHost = host;
    setName(tr("UDP Client (%1:%2)").arg(_sourceHost.toString()).arg(_port));
    emit linkChanged(this);

    if(reconnect)
    {
        connect();
    }
}

void UDPLink::setPort(int port)
{
    bool reconnect = false;
    if(isConnected())
    {
        disconnect();
        reconnect = true;
    }
    _port = port;
    setName(tr("UDP Client (%1:%2)").arg(_sourceHost.toString()).arg(_port));
    emit linkChanged(this);

    if(reconnect)
    {
        connect();
    }
}

/**
 * @param host Hostname in standard formatting, e.g. localhost:14551 or 192.168.1.1:14551
 */
void UDPLink::addHost(const QString& host)
{
    QLOG_INFO() << "UDP:" << "ADDING HOST:" << host;
//    if (host.contains(":"))
//    {
//        QLOG_DEBUG() << "HOST: " << host.split(":").first();
//        QHostInfo info = QHostInfo::fromName(host.split(":").first());
//        if (info.error() == QHostInfo::NoError)
//        {
//            // Add host
//            QList<QHostAddress> hostAddresses = info.addresses();
//            QHostAddress address;
//            for (int i = 0; i < hostAddresses.size(); i++)
//            {
//                // Exclude loopback IPv4 and all IPv6 addresses
//                if (!hostAddresses.at(i).toString().contains(":"))
//                {
//                    address = hostAddresses.at(i);
//                }
//            }
//            hosts.append(address);
//            QLOG_DEBUG() << "Address:" << address.toString();
//            // Set port according to user input
//            ports.append(host.split(":").last().toInt());
//        }
//    }
//    else
//    {
//        QHostInfo info = QHostInfo::fromName(host);
//        if (info.error() == QHostInfo::NoError)
//        {
//            // Add host
//            hosts.append(info.addresses().first());
//            // Set port according to default (this port)
//            ports.append(port);
//        }
//    }
//    emit linkChanged(this);
}

void UDPLink::removeHost(const QString& hostname)
{
    QLOG_INFO() << "UDP:" << "REMOVE HOST:" << hostname;
//    QString host = hostname;
//    if (host.contains(":")) host = host.split(":").first();
//    host = host.trimmed();
//    QHostInfo info = QHostInfo::fromName(host);
//    QHostAddress address;
//    QList<QHostAddress> hostAddresses = info.addresses();
//    for (int i = 0; i < hostAddresses.size(); i++)
//    {
//        // Exclude loopback IPv4 and all IPv6 addresses
//        if (!hostAddresses.at(i).toString().contains(":"))
//        {
//            address = hostAddresses.at(i);
//        }
//    }
//    for (int i = 0; i < hosts.count(); ++i)
//    {
//        if (hosts.at(i) == address)
//        {
//            hosts.removeAt(i);
//            ports.removeAt(i);
//        }
//    }
}

QList<QHostAddress> UDPLink::getHosts() const
{
    return _targetHosts;
}

QList<quint16> UDPLink::getPorts() const
{
    return _targetPorts;
}

void UDPLink::writeBytes(const char* data, qint64 size)
{
    if (!_socket.isOpen())
        return;

//    _socket.writeDatagram(data,size,_targetHost,_port);
    _socket.write(data, size);

    // Log the amount and time written out for future data rate calculations.
    QMutexLocker dataRateLocker(&dataRateMutex);
    logDataRateToBuffer(outDataWriteAmounts, outDataWriteTimes, &outDataIndex, size, QDateTime::currentMSecsSinceEpoch());
}

/**
 * @brief Read a number of bytes from the interface.
 *
 * @param data Pointer to the data byte array to write the bytes to
 * @param maxLength The maximum number of bytes to write
 **/
void UDPLink::readBytes()
{
    while (_socket.bytesAvailable())
    {
        _packetsReceived = true;

        QByteArray datagram;
        datagram.resize(_socket.bytesAvailable());

        _socket.readDatagram(datagram.data(), datagram.size());

        emit bytesReceived(this, datagram);

        // Log this data reception for this timestep
        QMutexLocker dataRateLocker(&dataRateMutex);
        logDataRateToBuffer(inDataWriteAmounts, inDataWriteTimes, &inDataIndex, datagram.length(), QDateTime::currentMSecsSinceEpoch());
    }
}

/**
 * @brief Get the number of bytes to read.
 *
 * @return The number of bytes to read
 **/
qint64 UDPLink::bytesAvailable()
{
    return _socket.bytesAvailable();
}

/**
 * @brief Disconnect the connection.
 *
 * @return True if connection has been disconnected, false if connection couldn't be disconnected.
 **/
bool UDPLink::disconnect()
{
    QLOG_INFO() << "UDP disconnect";
    quit();
    wait();

    _socket.close();
    _packetsReceived = true;

    emit disconnected();
    emit connected(false);
    emit disconnected(this);
    emit linkChanged(this);
    return false;
}

/**
 * @brief Connect the connection.
 *
 * @return True if connection has been established, false if connection couldn't be established.
 **/
bool UDPLink::connect()
{
    if (_socket.isOpen())
            disconnect();
    QLOG_INFO() << "UDPClientLink::UDP connect " << _sourceHost.toString() << ":" << _port;
    bool connected = _hardwareConnect();
    if (connected){
        start(NormalPriority);
    }
    return connected;
}

bool UDPLink::_hardwareConnect()
{

    if (!_sourceHost.isNull() && _port!=0) {
        QLOG_INFO() << "Connected UDP socket:" << _sourceHost.toString();
        if (_socket.bind(_sourceHost,_port)){

            emit connected(true);
            emit connected(this);
            emit connected();
            //QTimer::singleShot(5000, this, SLOT(_sendTriggerMessage()));
            return true;
        }
    }

    QLOG_ERROR() << "connect failed! " << _sourceHost.toString() << ":" << _port
                 << " err:" << _socket.error() << ": " << _socket.errorString();
    emit connected(false);
    emit disconnected(this);
    emit disconnected();
    return false;
}

void UDPLink::_sendTriggerMessage()
{
    if (!_packetsReceived){
        QLOG_DEBUG() << "Send UDP Client HELLO" << _sourceHost.toString();
        _socket.write("HELLO");
        QTimer::singleShot(5000, this, SLOT(_sendTriggerMessage()));
    }
}

void UDPLink::_socketError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError);
    emit communicationError(getName(), "UDP error on socket: " + _socket.errorString());
}

void UDPLink::_socketDisconnected()
{
    qDebug() << _name << ": disconnected";
    _socket.close();

    emit disconnected();
    emit connected(false);
    emit disconnected(this);
    emit linkChanged(this);
}

/**
 * @brief Check if connection is active.
 *
 * @return True if link is connected, false otherwise.
 **/
bool UDPLink::isConnected() const
{
    return _socket.isOpen();
}

int UDPLink::getId() const
{
    return _linkId;
}

quint16 UDPLink::getPort() const
{
    return _port;
}

QString UDPLink::getName() const
{
    return _name;
}

QString UDPLink::getShortName() const
{
    return QString("UDP Link");
}

QString UDPLink::getDetail() const
{
    return QString::number(_port);
}

void UDPLink::setName(QString name)
{
    _name = name;
    emit nameChanged(_name);
    emit linkChanged(this);
}


qint64 UDPLink::getConnectionSpeed() const
{
    return 54000000; // 54 Mbit
}

qint64 UDPLink::getCurrentInDataRate() const
{
    return 0;
}

qint64 UDPLink::getCurrentOutDataRate() const
{
    return 0;
}
