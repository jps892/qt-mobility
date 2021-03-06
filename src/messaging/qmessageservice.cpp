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
#include "qmessageservice.h"
#ifdef Q_OS_SYMBIAN
#include "qmessageservice_symbian_p.h"
#endif


QTM_BEGIN_NAMESPACE

/*!
    \class QMessageService


    \inmodule QtMessaging

    \ingroup messaging
    \since 1.0

    \brief The QMessageService class provides the interface for requesting
    messaging service operations.

    QMessageService provides the mechanisms for messaging clients to request services,
    and to receive information in response.  All requestable service operations present
    the same interface for communicating status, and progress information.

    All service request operations communicate changes in their operational state
    by emitting the stateChanged() signal.

    Service request operations report progress information by emitting the
    progressChanged() signal.

    If a requested operation fails after being initiated then the error()
    function will return a value indicating the failure mode encountered.

    A client may attempt to cancel a requested operation after it has been
    initiated. The cancel() slot is provided for this purpose.  Cancelation
    is not guaranteed to succeed, and is not possible on all platforms.

    A QMessageService instance supports only a single request at a time. Attempting
    to initiate an operation on a QMessageService while another operation is already
    in progress will result in function initiating the latter request returning
    false. A client may, however, use multiple QMessageService instances to
    concurrently initiate a queue of requests that will be serviced sequentially.

    Each QMessageService instance will report only the changes pertaining to
    the request that instance initiated.

    Except where noted QMessageServices may initiate network activity.
    Other functions in the mobility messaging API will not initiate network
    activity, with the exception of Windows mobile and desktop platforms which
    may initiate network activity during the evaluation of any function call.

    \warning On Symbian this class does not support QObject::moveToThread().
*/

/*!
    \enum QMessageService::State

    This enum type is used to describe the state of the requested operation.

    \value InactiveState    The operation has not yet begun execution.
    \value ActiveState      The operation is currently executing.
    \value CanceledState    The operation was canceled.
    \value FinishedState    The operation has finished execution; successfully completed or otherwise.
*/

/*!
    \fn QMessageService::QMessageService(QObject *parent)

    Constructs a message service representation object.

    The \a parent is passed to the QObject constructor.
*/

/*!
    \fn QMessageService::~QMessageService()

    Destroys the message service representation.
*/

/*!
    \fn QMessageService::queryMessages(const QMessageFilter &filter, const QMessageSortOrder &sortOrder, uint limit, uint offset)

    Emits the messagesFound() signal providing \l{QMessageId}s of messages in the messaging
    store. If \a filter is not empty only identifiers for messages matching the parameters
    set by \a filter will be emitted, otherwise identifiers for all messages will be emitted.
    If \a sortOrder is not empty, then the identifiers will be sorted by the parameters
    set by \a sortOrder.
    If \a limit is not zero, then \a limit places an upper bound on the number of
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.

    Calling this function may result in the messagesFound() and progressChanged()
    signals  being emitted multiple times. An emission of the progressChanged()
    signal with a total of 0 indicates that the number of progress steps is
    unknown.

    The performance of querying messages is currently significantly less than
    optimal for some querying criteria on some platforms.

    Returns true if the operation can be initiated; otherwise returns false.

    Note: This function should not initiate network activity, instead only message data
    already stored on the device will be used during evaluation of the function.

    \since 1.0
    \sa  QMessage, QMessageFilter, QMessageSortOrder, messagesFound(), progressChanged(), countMessages()
*/

/*!
    \fn QMessageService::queryMessages(const QMessageFilter &filter, const QString &body, QMessageDataComparator::MatchFlags matchFlags, const QMessageSortOrder &sortOrder, uint limit, uint offset)

    Emits the messagesFound() signal providing \l{QMessageId}s of messages in the messaging
    store. If \a filter is not empty only identifiers for messages matching the parameters
    set by \a filter and with a body containing the string \a body will be emitted,
    otherwise identifiers for all messages with a body containing \a body using
    matching flags \a matchFlags will be emitted.
    If \a sortOrder is not empty, then the identifiers will be sorted by the parameters
    set by \a sortOrder.
    If \a limit is not zero, then \a limit places an upper bound on the number of
    ids in the list returned.
    \a offset specifies how many ids to skip at the beginning of the list returned.

    Calling this function may result in the messagesFound() and progressChanged()
    signals being emitted multiple times. An emission of the progressChanged()
    signal with a total of 0 indicates that the number of progress steps is
    unknown.

    The performance of querying messages is currently significantly less than
    optimal for some querying criteria on some platforms.

    Returns true if the operation can be initiated; otherwise returns false.

    Note: This function should not initiate network activity, instead only message data
    already stored on the device will be used during evaluation of the function.

    \since 1.0
    \sa  QMessage, QMessageFilter, QMessageSortOrder, messagesFound(), progressChanged(), countMessages()
*/

/*!
    \fn QMessageService::countMessages(const QMessageFilter &filter)

    Emits via a messagesCounted() signal the number messages in the messaging
    store matching \a filter.

    Calling this function may result in the progressChanged()
    signal  being emitted multiple times. An emission of the progressChanged()
    signal with a total of 0 indicates that the number of progress steps is
    unknown.

    The performance of counting messages is currently significantly less than optimal
    for some filters on some platforms.

    Returns true if the operation can be initiated; otherwise returns false.

    Note: This function should not initiate network activity, instead only message data
    already stored on the device will be used during evaluation of the function.

    \since 1.0
    \sa  QMessage, QMessageFilter, messagesCounted(), progressChanged(), queryMessages()
*/

/*!
    \fn QMessageService::send(QMessage &message)

    Transmit \a message using the account identified by the message's \l{QMessage::parentAccountId()}{parentAccountId} function.
    If the message does not have a valid parentAccountId, it will be set to the
    result of QMessageAccount::defaultAccount() for the message's \l{QMessage::Type}{type}.

    The message will be stored in the standard Outbox folder for the account before
    transmission, or moved to that folder if it is already stored in another folder.
    If transmission is successful, the message will be moved to the standard Sent
    folder for the account.

    Currently on the Maemo 5 (Fremantle), Symbian, and Windows Mobile platforms the service
    action object enters the FinishedState when the message is queued for sending
    rather than actually sent.

    On the MeeGo.com platform this function is not yet supported for SMS type messages.

    Returns true if the operation can be initiated; otherwise returns false.

    \since 1.0
    \sa QMessage, QMessageAccountId
*/

/*!
    \fn QMessageService::compose(const QMessage &message)

    Open a composer application using \a message as a prototype.

    The default application for handling the type of \a message should be used.

    Returns true if the operation can be initiated; otherwise returns false.

    On the QMF platform implementation of this function is left as a task for system
    integrators.

    \since 1.0
    \sa QMessage::type()
*/

/*!
    \fn QMessageService::retrieveHeader(const QMessageId& id)

    Retrieve meta data of the message identified by \a id.

    The meta data (including flags, from, to, subject, and date fields where applicable) of
    the message identified by \a id should be retrieved.  If only the message envelope
    information is present, any other accessible header information should be retrieved.

    If the message can not be found on the originating server it will be marked as removed.

    Returns true if the operation can be initiated; otherwise returns false.
    
    On the Symbian^3 platform this function is not yet supported.
    
    \sa QMessageId, QMessage::Removed
*/

/*!
    \fn QMessageService::retrieveBody(const QMessageId& id)

    Retrieve the body of the message identified by \a id.

    If the message can not be found on the originating server it will be marked as removed.

    Returns true if the operation can be initiated; otherwise returns false.

    \since 1.0
    \sa QMessageId, QMessage::Removed
*/

/*!
    \fn QMessageService::retrieve(const QMessageId &messageId, const QMessageContentContainerId& id)

    Retrieve the container identified by \a messageId and \a id, the contents of the container should also be
    retrieved.

    Returns true if the operation can be initiated; otherwise returns false.

    \since 1.0
    \sa QMessageContentContainerId
*/

/*!
    \fn QMessageService::show(const QMessageId& id)

    Show the message identified by \a id.

    The default application for handling the type of message that \a id identifies should be used.

    Returns true if the operation can be initiated; otherwise returns false.

    On the Maemo 5 (Fremantle) platform this function is not yet supported for SMS type messages.

    On the QMF platform implementation of this function is left as a task for system
    integrators.

    \since 1.0
    \sa QMessageId, QMessage::type()
*/

/*!
    \fn QMessageService::exportUpdates(const QMessageAccountId &id)

    Initiate synchronization with external servers of local changes that have been queued by message store operations,
    for messages with parent account \a id.

    On Windows mobile and desktop platforms this function performs no operation, as when a connection is available,
    local changes are opportunistically synchronized with external servers.

    Returns true if the operation can be initiated; otherwise returns false.

    \since 1.0
    \sa QMessageManager::addMessage(), QMessageManager::updateMessage(), QMessageManager::removeMessage(), QMessageManager::removeMessages()
*/

/*!
    \fn QMessageService::moveMessages(const QMessageIdList &messageIds, const QMessageFolderId &toFolderId)

    Initiate a request to move messages \a messageIds to the folder \a toFolderId

    The folder must be in the same account as the message. This will move the message on the external server, as well as locally.

    On Windows mobile and desktop platforms this function is not supported. On Symbian prior to Belle this function is not supported.
    On Symbian Belle POP supports moving messages to drafts folder only and source folder must be either sent or outbox folder in
    IMAP and POP if target is drafts folder.

    Returns true if the operation can be initiated; otherwise returns false.
    \since 1.2
*/

/*!
    \fn QMessageService::synchronize(const QMessageAccountId &id)

    Initiate synchronization with external server identified by \a id

    On Windows mobile and desktop platforms, and Maemo 5 this function performs no operation.

    Returns true if the operation can be initiated; otherwise returns false.

    \since 1.2
    \sa QMessageManager::addMessage(), QMessageManager::updateMessage(), QMessageManager::removeMessage(), QMessageManager::removeMessages()
*/

/*!
    \fn QMessageService::state() const

    Returns the current state of the operation.

    \since 1.0
    \sa stateChanged()
*/

/*!
    \fn QMessageService::cancel()

    Attempts to cancel the last requested operation.
    \since 1.0
*/

/*!
    \fn QMessageService::stateChanged(QMessageService::State newState)

    This signal is emitted when the state of the operation changes, with the new state described by \a newState.

    \since 1.0
    \sa state()
*/

/*!
    \fn QMessageService::messagesFound(const QMessageIdList &ids);

    This signal is emitted when a queryMessages() operation has found
    messages.

    \a ids is the list of identifiers of messages found.

    \since 1.0
    \sa queryMessages()
*/

/*!
    \fn QMessageService::messagesCounted(int count);

    This signal is emitted when a countMessages() operation has counted
    messages.

    \a count is the number of matching messages found.

    \since 1.0
    \sa queryMessages()
*/

/*!
    \fn QMessageService::progressChanged(uint value, uint total)

    This signal is emitted when the operation has progressed.

    \a total is the total number of progress steps to perform, or zero if
    the number of progress steps is unknown.

    \a value is the number of progress steps completed so far.
    \since 1.0
*/

/*!
    \fn QMessageService::error() const

    Returns a value indicating the last error condition encountered by the operation.
    \since 1.0
*/

#include "moc_qmessageservice.cpp"
QTM_END_NAMESPACE
