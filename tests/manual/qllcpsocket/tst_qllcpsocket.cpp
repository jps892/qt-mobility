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

#include <QtTest/QtTest>

#include "../../auto/qconnectivitytestcommon.h"

#include <qllcpsocket.h>

#include <QtCore/QDebug>

QTM_USE_NAMESPACE

Q_DECLARE_METATYPE(QLlcpSocket::SocketState)
Q_DECLARE_METATYPE(QLlcpSocket::SocketError)

class tst_QLlcpSocket : public QObject
{
    Q_OBJECT

public:
    enum ClientConnectionShutdown {
        ServerDisconnect,
        ServerClose,
        ClientDisconnect,
        ClientClose
    };

    tst_QLlcpSocket();
    ~tst_QLlcpSocket();

private slots:
    void tst_construction();

    void tst_clientConnection_data();
    void tst_clientConnection();

    void tst_waitFor_data();
    void tst_waitFor();

    void tst_invalidServiceUri();

    void tst_serverConnectionless();
    void tst_clientConnectionless();
};

Q_DECLARE_METATYPE(tst_QLlcpSocket::ClientConnectionShutdown)

tst_QLlcpSocket::tst_QLlcpSocket()
{
    qRegisterMetaType<QLlcpSocket::SocketState>("QLlcpSocket::SocketState");
    qRegisterMetaType<QLlcpSocket::SocketError>("QLlcpSocket::SocketError");
}

tst_QLlcpSocket::~tst_QLlcpSocket()
{
}

void tst_QLlcpSocket::tst_construction()
{
    QLlcpSocket socket;

    QVERIFY(!socket.isOpen());
    QCOMPARE(socket.state(), QLlcpSocket::UnconnectedState);
    QCOMPARE(socket.error(), QLlcpSocket::UnknownSocketError);

    QVERIFY(!socket.hasPendingDatagrams());
    QCOMPARE(socket.pendingDatagramSize(), Q_INT64_C(-1));

    QCOMPARE(socket.bytesAvailable(), Q_INT64_C(0));
    QVERIFY(!socket.canReadLine());

    QVERIFY(socket.isSequential());
}

void tst_QLlcpSocket::tst_clientConnection_data()
{
    QTest::addColumn<ClientConnectionShutdown>("shutdown");
    QTest::addColumn<bool>("stream");

    QTest::newRow("server disconnect stream") << ServerDisconnect << true;
    QTest::newRow("server disconnect datagram") << ServerDisconnect << false;
    QTest::newRow("server close stream") << ServerClose << true;
    QTest::newRow("server close datagram") << ServerClose << false;
    QTest::newRow("client disconnect stream") << ClientDisconnect << true;
    QTest::newRow("client disconnect datagram") << ClientDisconnect << false;
    QTest::newRow("client close stream") << ClientClose << true;
    QTest::newRow("client close datagram") << ClientClose << false;
}

void tst_QLlcpSocket::tst_clientConnection()
{
    QFETCH(ClientConnectionShutdown, shutdown);
    QFETCH(bool, stream);

    QString service = QLatin1String("urn:nfc:sn:com.nokia.qtmobility.commandserver") +
                      (stream ? QLatin1String(".stream") : QLatin1String(".datagram"));

    /* Construction */
    QLlcpSocket *socket = new QLlcpSocket;

    QSignalSpy stateSpy(socket, SIGNAL(stateChanged(QLlcpSocket::SocketState)));

    QCOMPARE(socket->state(), QLlcpSocket::UnconnectedState);

    /* Connection */
    QSignalSpy connectedSpy(socket, SIGNAL(connected()));
    QSignalSpy errorSpy(socket, SIGNAL(error(QLlcpSocket::SocketError)));

    socket->connectToService(0, service);

    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(stateSpy.takeFirst().at(0).value<QLlcpSocket::SocketState>(),
             QLlcpSocket::ConnectingState);
    QCOMPARE(socket->state(), QLlcpSocket::ConnectingState);

    stateSpy.clear();

    QTRY_VERIFY_TIMEOUT(30000, !connectedSpy.isEmpty() || !errorSpy.isEmpty());

    if (errorSpy.count() != 0) {
        qDebug() << errorSpy.takeFirst().at(0).toInt();
        QSKIP("Connection error", SkipSingle);
    }

    QCOMPARE(connectedSpy.count(), 1);
    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(stateSpy.takeFirst().at(0).value<QLlcpSocket::SocketState>(),
             QLlcpSocket::ConnectedState);
    QCOMPARE(socket->state(), QLlcpSocket::ConnectedState);

    stateSpy.clear();

    QSignalSpy bytesWrittenSpy(socket, SIGNAL(bytesWritten(qint64)));
    QSignalSpy readyReadSpy(socket, SIGNAL(readyRead()));

    /* Verify connected to correct service */
    if (stream) {
        socket->write("URI\n");

        QTRY_VERIFY(!bytesWrittenSpy.isEmpty());

        QCOMPARE(bytesWrittenSpy.count(), 1);
        QCOMPARE(bytesWrittenSpy.takeFirst().at(0).value<qint64>(), qint64(4));

        QTRY_VERIFY(!readyReadSpy.isEmpty() && socket->canReadLine());

        const QByteArray line = socket->readLine().trimmed();

        QCOMPARE(line, service.toLatin1());
    } else {
        socket->writeDatagram("URI");

        QTRY_VERIFY(!bytesWrittenSpy.isEmpty());

        QCOMPARE(bytesWrittenSpy.count(), 1);
        QCOMPARE(bytesWrittenSpy.takeFirst().at(0).value<qint64>(), qint64(3));

        QTRY_VERIFY(!readyReadSpy.isEmpty() && socket->hasPendingDatagrams());

        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());

        socket->readDatagram(datagram.data(), datagram.size());

        QCOMPARE(datagram, service.toLatin1());
    }

    bytesWrittenSpy.clear();
    readyReadSpy.clear();

    /* Read / Write */
    if (stream) {
        QByteArray data("ECHO Test data\n");

        // fill up the local outgoing buffer
        int count = 0;
        while (socket->write(data) == data.size()) { ++count; }

        QTRY_VERIFY(!bytesWrittenSpy.isEmpty());

        QCOMPARE(bytesWrittenSpy.count(), 1);
        QCOMPARE(bytesWrittenSpy.takeFirst().at(0).value<qint64>(), count * qint64(data.size()));

        QTRY_VERIFY(!readyReadSpy.isEmpty());

        const QByteArray line = socket->readLine().trimmed();

        QCOMPARE(line.constData(), "Test data");
    } else {
        socket->writeDatagram("ECHO Test data");

        QTRY_VERIFY(!bytesWrittenSpy.isEmpty());

        QCOMPARE(bytesWrittenSpy.count(), 1);
        QCOMPARE(bytesWrittenSpy.takeFirst().at(0).value<qint64>(), qint64(14));

        QTRY_VERIFY(!readyReadSpy.isEmpty());

        QByteArray datagram;
        datagram.resize(socket->pendingDatagramSize());

        socket->readDatagram(datagram.data(), datagram.size());

        QCOMPARE(datagram.constData(), "Test data");
    }

    bytesWrittenSpy.clear();
    readyReadSpy.clear();

    QSignalSpy disconnectedSpy(socket, SIGNAL(disconnected()));
    errorSpy.clear();

    /* Shutdown */
    switch (shutdown) {
    case ServerDisconnect:
        if (stream)
            socket->write("DISCONNECT\n");
        else
            socket->writeDatagram("DISCONNECT");
        break;
    case ServerClose:
        if (stream)
            socket->write("CLOSE\n");
        else
            socket->writeDatagram("CLOSE");
        break;
    case ClientDisconnect:
        socket->disconnectFromService();
        break;
    case ClientClose:
        socket->close();
        break;
    }

    QTRY_VERIFY(!disconnectedSpy.isEmpty());

    QCOMPARE(disconnectedSpy.count(), 1);
    QCOMPARE(stateSpy.count(), 2);
    QCOMPARE(stateSpy.takeFirst().at(0).value<QLlcpSocket::SocketState>(),
             QLlcpSocket::ClosingState);
    QCOMPARE(stateSpy.takeFirst().at(0).value<QLlcpSocket::SocketState>(),
             QLlcpSocket::UnconnectedState);
    QVERIFY(!socket->isOpen());

    if (shutdown == ServerDisconnect || shutdown == ServerClose) {
        QTRY_VERIFY(!errorSpy.isEmpty());

        QCOMPARE(errorSpy.count(), 1);
        QCOMPARE(errorSpy.takeFirst().at(0).value<QLlcpSocket::SocketError>(),
                 QLlcpSocket::RemoteHostClosedError);
        QCOMPARE(socket->error(), QLlcpSocket::RemoteHostClosedError);
    }

    delete socket;
}

void tst_QLlcpSocket::tst_waitFor_data()
{
    QTest::addColumn<ClientConnectionShutdown>("shutdown");

    QTest::newRow("server disconnect") << ServerDisconnect;
    QTest::newRow("server close") << ServerClose;
    QTest::newRow("client disconnect") << ClientDisconnect;
    QTest::newRow("client close") << ClientClose;
}

void tst_QLlcpSocket::tst_waitFor()
{
    QFETCH(ClientConnectionShutdown, shutdown);

    QString service = QLatin1String("urn:nfc:sn:com.nokia.qtmobility.commandserver.stream");

    /* Construction */
    QLlcpSocket *socket = new QLlcpSocket;

    QCOMPARE(socket->state(), QLlcpSocket::UnconnectedState);

    /* Connection */
    socket->connectToService(0, service);
    QCOMPARE(socket->state(), QLlcpSocket::ConnectingState);
    QVERIFY(socket->waitForConnected());
    QCOMPARE(socket->state(), QLlcpSocket::ConnectedState);

    /* Verify connected to correct service */
    {
        socket->write("URI\n");
        QVERIFY(socket->waitForBytesWritten());

        QVERIFY(socket->waitForReadyRead());
        const QByteArray line = socket->readLine().trimmed();

        QCOMPARE(line, service.toLatin1());
    }

    /* Shutdown */
    switch (shutdown) {
    case ServerDisconnect:
        socket->write("DISCONNECT\n");
        break;
    case ServerClose:
        socket->write("CLOSE\n");
        break;
    case ClientDisconnect:
        socket->disconnectFromService();
        QCOMPARE(socket->state(), QLlcpSocket::ClosingState);
        break;
    case ClientClose:
        socket->close();
        QCOMPARE(socket->state(), QLlcpSocket::ClosingState);
        break;
    }

    QVERIFY(socket->waitForDisconnected());
    QVERIFY(!socket->isOpen());
    QCOMPARE(socket->state(), QLlcpSocket::UnconnectedState);

    if (shutdown == ServerDisconnect || shutdown == ServerClose)
        QCOMPARE(socket->error(), QLlcpSocket::RemoteHostClosedError);

    delete socket;
}

void tst_QLlcpSocket::tst_invalidServiceUri()
{
    QLatin1String invalidServiceUri("invalid");

    QLlcpSocket *socket = new QLlcpSocket;

    QSignalSpy stateSpy(socket, SIGNAL(stateChanged(QLlcpSocket::SocketState)));
    QSignalSpy errorSpy(socket, SIGNAL(error(QLlcpSocket::SocketError)));

    socket->connectToService(0, invalidServiceUri);

    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(stateSpy.takeFirst().at(0).value<QLlcpSocket::SocketState>(),
             QLlcpSocket::ConnectingState);
    QCOMPARE(socket->state(), QLlcpSocket::ConnectingState);

    QTRY_VERIFY(!errorSpy.isEmpty());

    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(stateSpy.takeFirst().at(0).value<QLlcpSocket::SocketState>(),
             QLlcpSocket::UnconnectedState);
    QCOMPARE(socket->state(), QLlcpSocket::UnconnectedState);

    QCOMPARE(errorSpy.takeFirst().at(0).value<QLlcpSocket::SocketError>(),
             QLlcpSocket::SocketAccessError);
    QCOMPARE(socket->error(), QLlcpSocket::SocketAccessError);
    QVERIFY(!socket->errorString().isEmpty());

    delete socket;
}

void tst_QLlcpSocket::tst_serverConnectionless()
{
    QLlcpSocket *socket = new QLlcpSocket;

    QSignalSpy stateSpy(socket, SIGNAL(stateChanged(QLlcpSocket::SocketState)));
    QSignalSpy errorSpy(socket, SIGNAL(error(QLlcpSocket::SocketError)));

    QVERIFY2(socket->bind(63), "Failed to bind to port 63");

    QVERIFY(errorSpy.isEmpty());
    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(stateSpy.takeFirst().at(0).value<QLlcpSocket::SocketState>(),
             QLlcpSocket::BoundState);
    QCOMPARE(socket->state(), QLlcpSocket::BoundState);

    QSignalSpy readyReadSpy(socket, SIGNAL(readyRead()));
    QTRY_VERIFY(!readyReadSpy.isEmpty());

    QVERIFY(socket->hasPendingDatagrams());

    while (socket->hasPendingDatagrams()) {
        qint64 size = socket->pendingDatagramSize();
        QVERIFY2(size > 0, "Invalid datagram size");

        QByteArray data;
        data.resize(size);
        QNearFieldTarget *target = 0;
        quint8 port = 0;

        qint64 readSize = socket->readDatagram(data.data(), size, &target, &port);

        QCOMPARE(size, readSize);
    }
}

void tst_QLlcpSocket::tst_clientConnectionless()
{
    QLlcpSocket *socket = new QLlcpSocket;

    QSignalSpy stateSpy(socket, SIGNAL(stateChanged(QLlcpSocket::SocketState)));
    QSignalSpy errorSpy(socket, SIGNAL(error(QLlcpSocket::SocketError)));

    QVERIFY2(socket->bind(0), "Failed to bind to port 0");

    QVERIFY(errorSpy.isEmpty());
    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(stateSpy.takeFirst().at(0).value<QLlcpSocket::SocketState>(),
             QLlcpSocket::BoundState);
    QCOMPARE(socket->state(), QLlcpSocket::BoundState);

    QSignalSpy bytesWrittenSpy(socket, SIGNAL(bytesWritten(qint64)));

    for (int i = 0; i < 60; ++i) {
        QString string = QString("Test message %1").arg(i);
        const QByteArray data = string.toUtf8();
        qint64 writeData = socket->writeDatagram(data, 0, 63);
        QCOMPARE(writeData, qint64(data.length()));

        QTest::qWait(500);
    }
}

QTEST_MAIN(tst_QLlcpSocket)

#include "tst_qllcpsocket.moc"

