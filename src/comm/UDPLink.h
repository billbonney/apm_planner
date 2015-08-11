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
 *   @brief UDP connection (server) for unmanned vehicles
 *   @author Lorenz Meier <mavteam@student.ethz.ch>
 *
 */

#ifndef UDPLINK_H
#define UDPLINK_H

#include <QString>
#include <QList>
#include <QMap>
#include <QMutex>
#include <QUdpSocket>
#include <LinkInterface.h>
#include <configuration.h>
#include <QQueue>
#include <QByteArray>
#include <QNetworkProxy>

class UDPLink : public LinkInterface
{
    Q_OBJECT
    //Q_INTERFACES(UDPLinkInterface:LinkInterface)

public:
    UDPLink(QHostAddress host = QHostAddress::Any, quint16 port = 14550);

    ~UDPLink();
    void disableTimeouts() { }
    void enableTimeouts() { }

    void requestReset() { }

    bool isConnected() const;
    qint64 bytesAvailable();
    quint16 getPort() const;

    /**
     * @brief The human readable port name
     */
    QString getName() const;
    QString getShortName() const;
    QString getDetail() const;
    int getBaudRate() const;
    int getBaudRateType() const;
    int getFlowType() const;
    int getParityType() const;
    int getDataBitsType() const;
    int getStopBitsType() const;
    QList<QHostAddress> getHosts() const;
    QList<quint16> getPorts() const;

    // Extensive statistics for scientific purposes
    qint64 getConnectionSpeed() const;
    qint64 getCurrentInDataRate() const;
    qint64 getCurrentOutDataRate() const;

    void run();

    int getId() const;

    LinkType getLinkType() { return UDP_LINK; }

public slots:
    void setAddress(QHostAddress host);
    void setPort(int port);

    /** @brief Add a new host to broadcast messages to */
    void addHost(const QString& host);
    /** @brief Remove a host from broadcasting messages to */
    void removeHost(const QString& host);

    void readBytes();
    /**
     * @brief Write a number of bytes to the interface.
     *
     * @param data Pointer to the data byte array
     * @param size The size of the bytes array
     **/
    void writeBytes(const char* data, qint64 length);

    bool connect();
    bool disconnect();

private slots:
    void _socketError(QAbstractSocket::SocketError socketError);
    void _socketDisconnected();

    void _sendTriggerMessage();

private: // Helper Methods
    void setName(QString name);
    bool _hardwareConnect();
    void _resetName();

private:
    QString _name;
    QHostAddress _sourceHost;
    quint16 _port;
    int _linkId;
    QUdpSocket _socket;
    bool _packetsReceived;

    QList<QHostAddress> _targetHosts;
    QList<quint16> _targetPorts;

    QMutex _dataMutex;
};

#endif // UDPLINK_H
