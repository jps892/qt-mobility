/****************************************************************************
**
** Copyright (C) 2009 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QEventLoop>
#include <QTimer>

#include "qnetworksession.h"

#ifdef Q_OS_SYMBIAN
#include "qnetworksession_s60_p.h"
#elif MAEMO
#include "qnetworksession_maemo_p.h"
#else
#include "qnetworksession_p.h"
#endif

QTM_BEGIN_NAMESPACE

/*!
    \class QNetworkSession

    \brief The QNetworkSession class provides control over the system's access points
    and enables session management for cases when multiple clients access the same access point.

    \inmodule QtNetwork
    \ingroup bearer

    A QNetworkSession enables control over the system's network interfaces. The session's configuration
    parameter are determined via the QNetworkConfiguration object to which it is bound. Depending on the 
    type of the session (single access point or service network) a session may be linked to one or more
    network interfaces. By means of \l{open()}{opening} and \l{close()}{closing} of network sessions 
    a developer can start and stop the systems network interfaces. If the configuration represents 
    multiple access points (see \l QNetworkConfiguration::ServiceNetwork) more advanced features such as roaming may be supported.

    QNetworkSession supports session management within the same process and depending on the platform's 
    capabilities may support out-of-process sessions. If the same 
    network configuration is used by multiple active sessions the underlying network interface is only terminated once
    the last session has been closed.

    \section1 Roaming

    Applications may connect to the preferredConfigurationChanged() signal in order to 
    receive notifications when a more suitable access point becomes available. 
    In response to this signal the application may initiate the roaming via migrate()
    or may ignore() the new access point. Once the session has roamed the 
    newConfigurationActivated() signal is emitted. The application may now test the 
    carrier and can accept() or reject() it. The session will return to the previous
    access point if the roaming was rejected.

    Some platforms may support the notion of forced roaming and application level roaming (ALR). 
    ALR implies that the application controls (via migrate(), ignore(), accept() and reject()) 
    whether a network session can roam from one network configuration to the next. Such control is useful
    if the application maintains stateful socket connections and wants to control the transition from
    one interface to the next.

    Forced roaming implies that the system automatically roams to the next network without 
    consulting the application. This has the advantage that the application can make use of roaming features
    without actually being aware of it. It is expected that the application detects that the underlying 
    socket is broken and automatically reconnects via the new network link.

    If the platform supports both modes of roaming an application indicates its preference
    by connecting to the preferredConfigurationChanged() signal. Connecting to this signal means that
    the application wants to take control over the roaming behavior and therefore implies application
    level roaming.

    \sa QNetworkConfiguration, QNetworkConfigurationManager
*/

/*!
    \enum QNetworkSession::State

    This enum describes the connectivity state of the session. If the session is based on a
    single access point configuration the state of the session is the same as the state of the
    associated network interface.

    \value Invalid          The session is invalid due to an invalid configuration. This may 
                            happen due to a removed access point or a configuration that was 
                            invalid to begin with.
    \value NotAvailable     The session is based on a defined but not yet discovered QNetworkConfiguration
                            (see \l QNetworkConfiguration::StateFlag).
    \value Connecting       The network session is being established.
    \value Connected        The network session is connected. If the current process wishes to use this session
                            it has to register its interest by calling open(). A network session 
                            is considered to be ready for socket operations if it isActive() and connected.
    \value Closing          The network session is in the process of being shut down.
    \value Disconnected     The network session is not connected. The associated QNetworkConfiguration
                            has the state QNetworkConfiguration::Discovered.
    \value Roaming          The network session is roaming from one access point to another 
                            access point.
*/

/*!
    \enum QNetworkSession::SessionError

    This enum describes the session errors that can occur.

    \value UnknownSessionError          An unidentified error occurred.
    \value SessionAbortedError          The session was aborted by the user or system.
    \value RoamingError                 The session cannot roam to the new configuration.
    \value OperationNotSupportedError   The operation is not supported for current configuration.
    \value InvalidConfigurationError    The operation cannot currently be performed for the
                                        current configuration.
*/

/*!
    \fn void QNetworkSession::stateChanged(QNetworkSession::State state)

    This signal is emitted whenever the state of the network session changes.
    The \a state parameter is the new state.

    \sa state()
*/

/*!
    \fn void QNetworkSession::error(QNetworkSession::SessionError error)

    This signal is emitted after an error occurred. The \a error parameter
    describes the error that occurred.

    \sa error(), errorString()
*/

/*!
    \fn void QNetworkSession::preferredConfigurationChanged(const QNetworkConfiguration& config, bool isSeamless)

    This signal is emitted when the preferred configuration/access point for the
    session changes. Only sessions which are based on service network configurations
    may emit this signal. \a config can be used to determine access point specific
    details such as proxy settings and \a isSeamless indicates whether roaming will
    break the sessions IP address.

    As a consequence to this signal the application may start the roaming process
    by calling migrate() or may chose to ignore() the new access point. If the application
    doesn't call either of the two functions the session ignores the migration opportunity.

    If the roaming process is non-seamless the IP address will change which means that
    a socket becomes invalid. However seamless mobility can ensure that the local IP address
    does not change. This is achieved by using a virtual IP address which is bound to the actual
    link address. During the roaming process the virtual address is attached to the new link 
    address.

    Some platforms may support the concept of Forced Roaming and Application Level Roaming (ALR). 
    Forced roaming implies that the platform may simply roam to a new configuration without 
    consulting applications. It is up to the application to detect the link layer loss and reestablish
    its sockets. In contrast ALR provides the opportunity to prevent the system from roaming. 
    If this session is based on a configuration that supports roaming the application can choose
    whether it wants to be consulted (ALR use case) by connecting to this signal. For as long as this signal 
    connection remains the session remains registered as a roaming stakeholder; otherwise roaming will 
    be enforced by the platform.

    \sa migrate(), ignore(), QNetworkConfiguration::roamingAvailable()
*/

/*!
    \fn void QNetworkSession::newConfigurationActivated()

    This signal is emitted once the session has roamed to the new access point.
    The application may reopen its socket and test the suitability of the new network link.
    Subsequently it may accept() or reject() the new access point. 

    \sa accept(), reject()
*/

/*!
    \fn void QNetworkSession::sessionOpened()

    This signal is emitted when the network session has been opened. 
    
    The underlying network interface will not be shut down as long as the session remains open.
    Note that this feature is dependent on \l{QNetworkConfigurationManager::SystemSessionSupport}{system wide session support}.
*/

/*!
    \fn void QNetworkSession::sessionClosed()

    This signal is emitted when the network session has been closed.
*/

/*!
    Constructs a session based on \a connectionConfig with the given \a parent.

    \sa QNetworkConfiguration
*/
QNetworkSession::QNetworkSession(const QNetworkConfiguration& connectionConfig, QObject* parent)
    : QObject(parent)
{
    d = new QNetworkSessionPrivate;
    d->q = this;
    d->publicConfig = connectionConfig;
    d->syncStateWithInterface();
    QObject::connect(d, SIGNAL(quitPendingWaitsForOpened()), 
                this, SIGNAL(sessionOpened()));
}

/*!
    Frees the resources associated with the QNetworkSession object.
*/
QNetworkSession::~QNetworkSession()
{
    delete d;
}

/*!
    Creates an active/open session which increases the session counter on the underlying network interface.
    The system will not terminate a network interface until the session reference counter reaches zero.
    Therefore an active session allows an application to register its use of the interface.

    The interface is started if it is not active yet. Some platforms may not provide support 
    for out-of-process sessions. On such platforms the session counter ignores any sessions
    held by another process. The platform capabilities can be 
    detected via QNetworkConfigurationManager::capabilities().

    Note that this call is asynchronous. Depending on the outcome of this call the results can be enquired 
    by connecting to the stateChanged(), sessionOpened() or error() signals.

    It is not a requirement to open a session in order to monitor the underlying network interface.

    \sa close(), stop(), isActive()
*/
void QNetworkSession::open()
{
    d->open();
}

/*!
    Waits until the session has been opened, up to \a msecs milliseconds. If the session has been opened, this
    function returns true; otherwise it returns false. In the case where it returns false, you can call error()
    to determine the cause of the error.

    The following example waits up to one second for the session to be opened:

    \code
        session->open();
        if (session->waitForOpened(1000))
            qDebug("Open!");
    \endcode

    If \a msecs is -1, this function will not time out.

    \sa open(), error()
*/
bool QNetworkSession::waitForOpened(int msecs)
{
    if (d->isActive)
        return true;

    if (d->state != Connecting)
        return false;

    QEventLoop* loop = new QEventLoop(this);
    QObject::connect(d, SIGNAL(quitPendingWaitsForOpened()),
                     loop, SLOT(quit()));

    //final call
    if (msecs>=0)
        QTimer::singleShot(msecs, loop, SLOT(quit()));

    loop->exec();
    loop->disconnect();
    loop->deleteLater();

    return d->isActive;
}

/*!
    Decreases the session counter on the associated network configuration. If the session counter reaches zero
    the active network interface is shut down. This also means that state() will only change from \l Connected to
    \l Disconnected if this was the last active session.

    If the platform does not support out-of-process sessions calling this function does not stop the
    interface. In this case \l{stop()} has to be used to force a shut down. 
    The platform capabilities can be detected via QNetworkConfigurationManager::capabilities().

    Note that this call is asynchronous. Depending on the outcome of this call the results can be enquired 
    by connecting to the stateChanged(), sessionOpened() or error() signals.

    \sa open(), stop(), isActive()
*/
void QNetworkSession::close()
{
    d->close();
}

/*!
    Invalidates all active sessions against the network interface and therefore stops the 
    underlying network interface. This function always changes the session's state() flag to
    \l Disconnected.

    \sa open(), close()
*/
void QNetworkSession::stop()
{
    d->stop();
}

/*!
    Returns the QNetworkConfiguration that this network session object is based on.

    \sa QNetworkConfiguration
*/
QNetworkConfiguration QNetworkSession::configuration() const
{
    return d->publicConfig;
}

/*!
    Returns the type of bearer currently used by this session. The string is not translated and therefore can 
    not be shown to the user. The subsequent table presents the currently known bearer types:

    \table
        \header 
            \o Value
            \o Description
        \row
            \o Ethernet
            \o The session is based on Ethernet.
        \row
            \o WLAN
            \o The session is based on Wireless LAN.
        \row
            \o 2G
            \o The session uses CSD, GPRS, HSCSD, EDGE or cdmaOne.
        \row 
            \o CDMA2000
            \o The session uses CDMA.
        \row
            \o WCDMA
            \o The session uses W-CDMA/UMTS.
        \row
            \o HSPA
            \o The session uses High Speed Packet Access.
        \row
            \o Bluetooth
            \o The session uses Bluetooth.
        \row
            \o WiMAX
            \o The session uses WiMAX.
    \endtable

    If the session is based on a network configuration of type 
    \l QNetworkConfiguration::ServiceNetwork the type of the preferred or currently 
    active configuration is returned. Therefore the bearer type may change 
    over time.

    This function returns an empty string if this session is based on an invalid configuration.
*/
QString QNetworkSession::bearerName() const
{
    return d->bearerName();
}

/*!
    Returns the network interface that is used by this session.

    This function only returns a valid QNetworkInterface when this session is \l Connected.

    The returned interface may change as a result of a roaming process.

    \sa state()
*/
QNetworkInterface QNetworkSession::interface() const
{
    return d->currentInterface();
}

/*!
    Returns true if this object holds an active session on the underlying network interface.
    The session can be controlled via open() and close().
*/
bool QNetworkSession::isActive() const
{
    return d->isActive;
}

/*!
    Returns the state of the session. If the session is based on a
    single access point configuration the state of the session is the same as the state of the
    associated network interface. Therefore a network session object can be used to monitor 
    network interfaces. 

    A \l QNetworkConfiguration::ServiceNetwork based session summarizes the state of all its children
    and therefore returns the \l Connected state if at least one of its sub configurations is connected. 

    Note that it is not required to hold an active session in order to obtain the network interface state.
    A connected but inactive session may be used to monitor network interfaces whereas an active and connected
    session object may prevent the network interface from being shut down.

    \sa error(), stateChanged()
*/
QNetworkSession::State QNetworkSession::state() const
{
    return d->state;
}

/*!
    Returns the type of error that last occurred.

    \sa state(), errorString()
*/
QNetworkSession::SessionError QNetworkSession::error() const
{
    return d->error();
}

/*!
    Returns a human-readable description of the last device error that 
    occurred.

    \sa error()
*/
QString QNetworkSession::errorString() const
{
    return d->errorString();
}

/*!
    Returns the value for property \a key.

    A network session can have properties attached which may describe the session in more details.
    This function can be used to gain access to those properties.

    The following property keys are guaranteed to be specified on all platforms:

    \table
        \header
            \o Key \o Description
        \row
            \o ActiveConfigurationIdentifier
            \o If the session \l isActive() this property returns the identifier of the
            QNetworkConfiguration that is used by this session; otherwise an empty string.

            The main purpose of this key is to determine which Internet access point is used
            if the session is based on a \l{QNetworkConfiguration::ServiceNetwork}{ServiceNetwork}. 
            The following code snippet highlights the difference:
            \code
                    QNetworkConfigurationManager mgr;
                    QNetworkConfiguration ap = mgr.defaultConfiguration();
                    QNetworkSession* session = new QNetworkSession(ap);
                    ... //code activates session

                    QString ident = session->property("ActiveConfigurationIdentifier").toString();
                    if ( ap.type() == QNetworkConfiguration::ServiceNetwork ) {
                        Q_ASSERT( ap.identifier() != ident );
                        Q_ASSERT( ap.children().contains( mgr.configurationFromIdentifier(ident) ) );
                    } else if ( ap.type() == QNetworkConfiguration::InternetAccessPoint ) {
                        Q_ASSERT( ap.identifier() == ident );
                    }
                \endcode
        \row
            \o UserChoiceConfigurationIdentifier
            \o If the session \l isActive() and is bound to a QNetworkConfiguration of type
            UserChoice, this property returns the identifier of the QNetworkConfiguration that the
            configuration resolved to when \l open() was called; otherwise an empty string.

            The purpose of this key is to determine the real QNetworkConfiguration that the
            session is using. This key is different to \i ActiveConfigurationIdentifier in that
            this key may return an identifier for either a
            \l {QNetworkConfiguration::ServiceNetwork}{service network} or a
            \l {QNetworkConfiguration::InternetAccessPoint}{Internet access points} configurations
            whereas \i ActiveConfigurationIdentifier always returns identifiers for
            \l {QNetworkConfiguration::InternetAccessPoint}{Internet access points} configurations.
        \row
            \o ConnectInBackground
            \o Setting this property to \i true before calling \l open() implies that the connection attempt
            is made but if no connection can be established, the user is not connsulted and asked to select
            a suitable connection. This property is not set by default and support for it depends on the platform.
    \endtable
*/
QVariant QNetworkSession::property(const QString& key) const
{
    if (!d->publicConfig.isValid())
        return QVariant();

    if (key == "ActiveConfigurationIdentifier") {
        if (!d->isActive)
            return QString();
        else
            return d->activeConfig.identifier();
    }

    if (key == "UserChoiceConfigurationIdentifier") {
        if (!d->isActive || d->publicConfig.type() != QNetworkConfiguration::UserChoice)
            return QString();

        if (d->serviceConfig.isValid())
            return d->serviceConfig.identifier();
        else
            return d->activeConfig.identifier();
    }

    return d->property(key);
}

/*!
    Sets the property \a value on the session. The property is identified using
    \a key. Removing an already set  property can be achieved by passing an 
    invalid QVariant.

    Note that the \i UserChoiceConfigurationIdentifier and \i ActiveConfigurationIdentifier
    properties are read only and cannot be changed using this method.
*/
void QNetworkSession::setProperty(const QString& key, const QVariant& value)
{
    if (key == "ActiveConfigurationIdentifier" 
            || key == "UserChoiceConfigurationIdentifier") 
        return;

    d->setProperty(key, value);
}

/*!
    Instructs the session to roam to the new access point. The old access point remains active 
    until the application calls accept().

   The newConfigurationActivated() signal is emitted once roaming has been completed.

    \sa accept()
*/
void QNetworkSession::migrate()
{
    d->migrate();
}

/*!
    This function indicates that the application does not wish to roam the session. This
    is the default behavior if an application doesn't call migrate() in response to a
    preferredConfigurationChanged() signal.

    \sa migrate()
*/
void QNetworkSession::ignore()
{
    //TODO Do we really need this function if we consider that this is
    //     the default behavior if nobody calls migrate()?
    d->ignore();
}

/*!
    Instructs the session to permanently accept the new access point. Once this function 
    has been called the session may not return to the old access point.

    The old access point may be closed in the process if there are no other network sessions for it.
    Therefore any open socket that still uses the old access point 
    may become unusable and should be closed before completing the migration.
*/
void QNetworkSession::accept()
{
    d->accept();
}

/*!
    The new access point is not suitable for the application. By calling this function the 
    session returns to the previous access point/configuration. This action may invalidate
    any socket that has been created via the not desired access point.

    \sa accept()
*/
void QNetworkSession::reject()
{
    d->reject();
}


/*!
    Returns the amount of data sent in bytes; otherwise 0.

    This field value includes the usage across all active network 
    sessions which use the same network interface.

    If the session is based on a service network configuration the number of 
    sent bytes across all active member configurations are returned.

    This function may not always be supported on all platforms and returns
    0. The platform capability can be detected via QNetworkConfigurationManager::DataStatistics.
*/
quint64 QNetworkSession::sentData() const
{
    return d->sentData();
}

/*!
    Returns the amount of data received in bytes; otherwise 0.

    This field value includes the usage across all active network 
    sessions which use the same network interface.

    If the session is based on a service network configuration the number of 
    sent bytes across all active member configurations are returned.

    This function may not always be supported on all platforms and returns
    0. The platform capability can be detected via QNetworkConfigurationManager::DataStatistics.
*/
quint64 QNetworkSession::receivedData() const
{
    return d->receivedData();
}

/*!
    Returns the number of seconds that the session has been active.
*/
quint64 QNetworkSession::activeTime() const
{
    return d->activeTime();
}

/*!
    \internal

    This function is required to detect whether the client wants to control 
    the roaming process. If he connects to preferredConfigurationChanged() signal
    he intends to influence it. Otherwise QNetworkSession always roams
    without registering this session as a stakeholder in the roaming process.

    For more details check the Forced vs ALR roaming section in the QNetworkSession 
    class description.
*/
void QNetworkSession::connectNotify(const char *signal)
{
    QObject::connectNotify(signal);
    //check for preferredConfigurationChanged() signal connect notification
    //This is not required on all platforms
#ifdef Q_OS_SYMBIAN
    if (qstrcmp(signal, SIGNAL(preferredConfigurationChanged(QNetworkConfiguration,bool))) == 0) {
        d->setALREnabled(true);
    }
#endif    
}

/*!
    \internal

    This function is called when the client disconnects from the preferredConfigurationChanged()
    signal.

    \sa connectNotify()
*/
void QNetworkSession::disconnectNotify(const char *signal)
{
    QObject::disconnectNotify(signal);
    //check for preferredConfigurationChanged() signal disconnect notification
    //This is not required on all platforms
#ifdef Q_OS_SYMBIAN
    if (qstrcmp(signal, SIGNAL(preferredConfigurationChanged(QNetworkConfiguration,bool))) == 0) {
        d->setALREnabled(false);
    }    
#endif    
}

#include "moc_qnetworksession.cpp"

QTM_END_NAMESPACE