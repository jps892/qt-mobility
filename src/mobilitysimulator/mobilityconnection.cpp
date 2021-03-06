/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "mobilitysimulatorglobal.h"
#include "mobilityconnection_p.h"
#include "private/qsimulatorconnection_p.h"

#include <QtCore/QCoreApplication>
#include <QtNetwork/QLocalServer>
#include <QtNetwork/QLocalSocket>

QTM_BEGIN_NAMESPACE

using namespace QtSimulatorPrivate;

namespace Simulator {

static const VersionStruct mobilityVersion(1, 1, 0, 0);

MobilityConnection::MobilityConnection(QObject *parent)
    : QObject(parent)
    , mSendSocket(0)
    , mReceiveSocket(0)
{

}

MobilityConnection::~MobilityConnection()
{
    delete mReceiveSocket;
    mReceiveSocket = 0;
}

MobilityConnection *MobilityConnection::instance()
{
    static MobilityConnection *gInstance = 0;
    if (!gInstance) {
        // ensure the simulator is up and running before trying to
        // connect to its mobility subserver
        SimulatorConnection::instance();

        gInstance = new MobilityConnection();
        gInstance->connectToSimulator();
    }
    return gInstance;
}

void MobilityConnection::addMessageHandler(QObject *handler)
{
    mHandlers.append(handler);
}

QLocalSocket *MobilityConnection::sendSocket()
{
    return mSendSocket;
}

QLocalSocket *MobilityConnection::receiveSocket()
{
    return mReceiveSocket;
}

void MobilityConnection::onReadyRead()
{
    mReadBuffer += mReceiveSocket->readAll();
    forever {
        IncomingRemoteMetacall rpc;
        if (rpc.read(&mReadBuffer)) {
            // first, check if this can call a slot on this
            if (rpc.call(mReceiveSocket, this))
                continue;
            // then try the registered handlers until one matches
            bool handled = false;
            foreach (QObject *handler, mHandlers) {
                if (rpc.call(mReceiveSocket, handler)) {
                    handled = true;
                    break;
                }
            }
            if (handled)
                continue;
            qWarning("Ignoring a call to %s,", rpc.signature().data());
        }
        break;
    }
}

void MobilityConnection::connectToSimulator()
{
    // 1. check availability
    QLocalSocket *socket = new QLocalSocket;
    QString simVersion = SimulatorConnection::instance()->simulatorVersion().toString();
    socket->connectToServer(QLatin1String(SIMULATOR_MOBILITY_SERVERNAME) + simVersion, QLocalSocket::ReadWrite);
    if (!socket->waitForConnected(1000)) {
        qFatal("Could not connect to mobility server");
        socket->deleteLater();
        return;
    }
    mSendSocket = socket;

    // 2. Create the local server used for initiating the backchannel.
    const qint64 pid = QCoreApplication::applicationPid();
    QLocalServer *server = new QLocalServer(this);
    if (!server->listen(qt_mobilityServerName(simVersion, pid)))
        qFatal("Can't create local mobility server for this application.");

    // 3. Send initial application connect command.
    ApplicationConnectCommand command;
    ApplicationConnectCommand::Request &request = command.request;
    request.applicationPid = pid;
    request.applicationName[0] = 0;
    request.version = mobilityVersion;
    qt_sendAndReceiveApplicationConnect(socket, command);

    mSimulatorVersion = command.reply.version;

    // We usually want to get the initial state from the simulator directly, probably
    // before the event loop is started up. Hence we block until the simulator has established
    // the back channel.
    if (!server->waitForNewConnection(1000))
        qFatal("Simulator didn't establish mobility-backchannel on time");
    mReceiveSocket = server->nextPendingConnection();
    server->close();

    connect(mReceiveSocket, SIGNAL(readyRead()), SLOT(onReadyRead()));
}

} // end namespace Simulator

#include "moc_mobilityconnection_p.cpp"

QTM_END_NAMESPACE
