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
#include "maemohelpers_p.h"
#include "qmessagefilter_p.h"
#include "qmessagesortorder_p.h"
#include "qmessageaccountfilter_p.h"
#include "qmessageaccountsortorder_p.h"
#include "qmessagefolderfilter_p.h"
#include "qmessagefoldersortorder_p.h"
#include "qmessagestore_p.h"

QTM_BEGIN_NAMESPACE

Q_GLOBAL_STATIC(MessageCache, messageCache);
Q_GLOBAL_STATIC(MessagingHelper, messagingHelper);

MessagingHelper* MessagingHelper::instance()
{
    return messagingHelper();
}

MessagingHelper::MessagingHelper()
{
}

MessagingHelper::~MessagingHelper()
{
}


void MessagingHelper::filterAccounts(QMessageAccountIdList& accountIds, const QMessageAccountFilter& filter)
{
    if (accountIds.count() && !filter.isEmpty()) {
        QMessageAccountFilterPrivate* pf = QMessageAccountFilterPrivate::implementation(filter);
        if ((pf->_field == QMessageAccountFilterPrivate::None) &&
            (pf->_filterList.count() == 0)) {
            if (pf->_notFilter) {
                // There is only one filter: empty ~QMessageFilter()
                // => accountIds must be cleared
                accountIds.clear();
                return;
            } else {
                // There is only one filter: empty QMessageFilter()
                // => accountIds list can remain intact
                return;
            }
        }

        if (pf->_valid) {
            QMessageStore* store = QMessageStore::instance();
            for (int i=accountIds.count()-1; i >= 0; i--) {
                QMessageAccount account = store->account(accountIds[i]);
                if (!pf->filter(account)) {
                    accountIds.removeAt(i);
                }
            }
        }
    }
}

bool MessagingHelper::accountLessThan(const QMessageAccountId accountId1, const QMessageAccountId accountId2)
{
    QMessageStore* store = QMessageStore::instance();
    return QMessageAccountSortOrderPrivate::lessThan(*messagingHelper()->m_AccountSortOrder,
                                                     store->account(accountId1),
                                                     store->account(accountId2));
}

void MessagingHelper::orderAccounts(QMessageAccountIdList& accountIds, const QMessageAccountSortOrder &sortOrder)
{
    if (!sortOrder.isEmpty()) {
        messagingHelper()->m_AccountSortOrder = (QMessageAccountSortOrder*)&sortOrder;
        qSort(accountIds.begin(), accountIds.end(), MessagingHelper::accountLessThan);
    }
}

void MessagingHelper::applyOffsetAndLimitToAccountIdList(QMessageAccountIdList& accountIds, int limit, int offset)
{
    if (accountIds.count() == 0) {
        return;
    }

    if (offset > 0) {
        if (offset > accountIds.count()) {
            accountIds.clear();
        } else {
            for (int i = 0; i < offset; i++) {
                accountIds.removeFirst();
            }
        }
    }
    if (limit > 0) {
        for (int i = accountIds.count()-1; i >= limit; i--) {
            accountIds.removeAt(i);
        }
    }
}

void MessagingHelper::filterFolders(QMessageFolderIdList& folderIds, const QMessageFolderFilter& filter)
{
    if (folderIds.count() && !filter.isEmpty()) {
        QMessageFolderFilterPrivate* pf = QMessageFolderFilterPrivate::implementation(filter);
        if ((pf->_field == QMessageFolderFilterPrivate::None) &&
            (pf->_filterList.count() == 0)) {
            if (pf->_notFilter) {
                // There is only one filter: empty ~QMessageFilter()
                // => accountIds must be cleared
                folderIds.clear();
                return;
            } else {
                // There is only one filter: empty QMessageFilter()
                // => accountIds list can remain intact
                return;
            }
        }

        if (pf->_valid) {
            QMessageStore* store = QMessageStore::instance();
            for (int i=folderIds.count()-1; i >= 0; i--) {
                QMessageFolder folder = store->folder(folderIds[i]);
                if (!pf->filter(folder)) {
                    folderIds.removeAt(i);
                }
            }
        }
    }
}

bool MessagingHelper::folderLessThan(const QMessageFolderId folderId1, const QMessageFolderId folderId2)
{
    QMessageStore* store = QMessageStore::instance();
    return QMessageFolderSortOrderPrivate::lessThan(*messagingHelper()->m_FolderSortOrder,
                                                    store->folder(folderId1),
                                                    store->folder(folderId2));
}

void MessagingHelper::orderFolders(QMessageFolderIdList& folderIds,  const QMessageFolderSortOrder &sortOrder)
{
    if (!sortOrder.isEmpty()) {
        messagingHelper()->m_FolderSortOrder = (QMessageFolderSortOrder*)&sortOrder;
        qSort(folderIds.begin(), folderIds.end(), MessagingHelper::folderLessThan);
    }
}

void MessagingHelper::applyOffsetAndLimitToFolderIdList(QMessageFolderIdList& folderIds, int limit, int offset)
{
    if (folderIds.count() == 0) {
        return;
    }

    if (offset > 0) {
        if (offset > folderIds.count()) {
            folderIds.clear();
        } else {
            for (int i = 0; i < offset; i++) {
                folderIds.removeFirst();
            }
        }
    }
    if (limit > 0) {
        for (int i = folderIds.count()-1; i >= limit; i--) {
            folderIds.removeAt(i);
        }
    }
}

void MessagingHelper::filterMessages(QMessageIdList& messageIds, const QMessageFilter& filter)
{
    if (messageIds.count() && !filter.isEmpty()) {
        QMessageFilterPrivate* pf = QMessageFilterPrivate::implementation(filter);
        if ((pf->_field == QMessageFilterPrivate::None) &&
            (pf->_filterList.count() == 0)) {
            if (pf->_notFilter) {
                // There is only one filter: empty ~QMessageFilter()
                // => messageIds must be cleared
                messageIds.clear();
                return;
            } else {
                // There is only one filter: empty QMessageFilter()
                // => messageIds list can remain intact
                return;
            }
        }

        if (pf->_valid) {
            MessageCache* cache = MessageCache::instance();
            QMessageStore* store = QMessageStore::instance();
            QMessage* msg;
            for (int i=messageIds.count()-1; i >= 0; i--) {
                cache->lock();
                msg = cache->messageObject(messageIds[i]);
                if (msg) {
                    if (!pf->filter(*msg)) {
                        messageIds.removeAt(i);
                    }
                    cache->unlock();
                } else {
                    cache->unlock();
                    if (!pf->filter(store->message(messageIds[i]))) {
                        messageIds.removeAt(i);
                    }
                }
            }
        }
    }
}

bool MessagingHelper::messageLessThan(const QMessageId messageId1, const QMessageId messageId2)
{
    bool retVal = false;

    MessageCache* cache = MessageCache::instance();
    cache->lock();
    QMessage *msg1 = cache->messageObject(messageId1);
    QMessage *msg2 = cache->messageObject(messageId2);

    if (msg1 == 0 || msg2 == 0) {
        QMessageStore* store = QMessageStore::instance();
        if (msg1 != 0) {
            cache->unlock();
            QMessage message2 = store->message(messageId2);
            cache->lock();
            retVal = QMessageSortOrderPrivate::lessThan(*messagingHelper()->m_MessageSortOrder,
                                                        *msg1,
                                                        message2);
            cache->unlock();
        } else if (msg2 != 0) {
            cache->unlock();
            QMessage message1 = store->message(messageId1);
            cache->lock();
            retVal = QMessageSortOrderPrivate::lessThan(*messagingHelper()->m_MessageSortOrder,
                                                        message1,
                                                        *msg2);
            cache->unlock();
        } else {
            cache->unlock();
            QMessage message1 = store->message(messageId1);
            QMessage message2 = store->message(messageId2);
            retVal = QMessageSortOrderPrivate::lessThan(*messagingHelper()->m_MessageSortOrder,
                                                        message1,
                                                        message2);
        }
    } else {
        retVal = QMessageSortOrderPrivate::lessThan(*messagingHelper()->m_MessageSortOrder, *msg1, *msg2);
        cache->unlock();
    }

    return retVal;
}

void MessagingHelper::orderMessages(QMessageIdList& messageIds,  const QMessageSortOrder &sortOrder)
{
    if (!sortOrder.isEmpty()) {
        messagingHelper()->m_MessageSortOrder = (QMessageSortOrder*)&sortOrder;
        qSort(messageIds.begin(), messageIds.end(), MessagingHelper::messageLessThan);
    }
}

void MessagingHelper::applyOffsetAndLimitToMessageIdList(QMessageIdList& messageIds, int limit, int offset)
{
    if (messageIds.count() == 0) {
        return;
    }

    if (offset > 0) {
        if (offset > messageIds.count()) {
            messageIds.clear();
        } else {
            for (int i = 0; i < offset; i++) {
                messageIds.removeFirst();
            }
        }
    }
    if (limit > 0) {
        for (int i = messageIds.count()-1; i >= limit; i--) {
            messageIds.removeAt(i);
        }
    }
}

void MessagingHelper::handleNestedFiltersFromFolderFilter(QMessageFolderFilter &filter)
{
    QMessageStore* store = QMessageStore::instance();

    QMessageFolderFilterPrivate* pMFFilter = QMessageFolderFilterPrivate::implementation(filter);
    if (pMFFilter->_filterList.count() > 0) {
        int filterListCount = pMFFilter->_filterList.count();
        for (int i=0; i < filterListCount; i++) {
            for (int j=0; j < pMFFilter->_filterList[i].count(); j++) {
                QMessageFolderFilterPrivate* pMFFilter2 = QMessageFolderFilterPrivate::implementation(pMFFilter->_filterList[i][j]);
                if (pMFFilter2->_field == QMessageFolderFilterPrivate::ParentAccountIdFilter) {
                    QMessageAccountIdList accountIds = store->queryAccounts(*pMFFilter2->_accountFilter, QMessageAccountSortOrder(), 0, 0);
                    QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter2->_comparatorValue));
                    if (accountIds.count() > 0) {
                        pMFFilter->_filterList[i].removeAt(j);
                        if (cmp == QMessageDataComparator::Includes) {
                            for (int x = 0; x < accountIds.count(); x++) {
                                if (x == 0) {
                                    if (x+1 < accountIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[i]);
                                    }
                                    pMFFilter->_filterList[i].append(QMessageFolderFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                    qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFolderFilterPrivate::lessThan);
                                } else {
                                    if (x+1 < accountIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[pMFFilter->_filterList.count()-1]);
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-2].append(QMessageFolderFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-2].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-2].end(), QMessageFolderFilterPrivate::lessThan);
                                    } else {
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-1].append(QMessageFolderFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-1].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-1].end(), QMessageFolderFilterPrivate::lessThan);
                                    }
                                }
                            }
                        } else { // Excludes
                            for (int x = 0; x < accountIds.count(); x++) {
                                pMFFilter->_filterList[i].append(QMessageFolderFilter::byParentAccountId(accountIds[x],QMessageDataComparator::NotEqual));
                            }
                            qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFolderFilterPrivate::lessThan);
                        }
                    } else {
                        delete pMFFilter2->_accountFilter;
                        pMFFilter2->_accountFilter = 0;
                        pMFFilter2->_field = QMessageFolderFilterPrivate::Id;
                        qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFolderFilterPrivate::lessThan);
                    }
                } else {
                    break;
                }
            }
        }
    } else {
        if (pMFFilter->_field == QMessageFolderFilterPrivate::ParentAccountIdFilter) {
            QMessageAccountIdList accountIds = store->queryAccounts(*pMFFilter->_accountFilter, QMessageAccountSortOrder(), 0, 0);
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter->_comparatorValue));
            if (accountIds.count() > 0) {
                for (int i=0; i < accountIds.count(); i++) {
                    if (i == 0) {
                        delete pMFFilter->_accountFilter;
                        pMFFilter->_accountFilter = 0;
                        pMFFilter->_field = QMessageFolderFilterPrivate::ParentAccountId;
                        pMFFilter->_value = accountIds[0].toString();
                        pMFFilter->_comparatorType = QMessageFolderFilterPrivate::Equality;
                        if (cmp == QMessageDataComparator::Includes) {
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::Equal);
                        } else { // Excludes
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::NotEqual);
                        }
                    } else {
                        if (cmp == QMessageDataComparator::Includes) {
                            filter |= QMessageFolderFilter::byParentAccountId(accountIds[i],QMessageDataComparator::Equal);
                        } else { // Excludes
                            filter &= QMessageFolderFilter::byParentAccountId(accountIds[i],QMessageDataComparator::NotEqual);
                        }
                    }
                }
            } else {
                delete pMFFilter->_accountFilter;
                pMFFilter->_accountFilter = 0;
                pMFFilter->_field = QMessageFolderFilterPrivate::Id;
            }
        }
    }
}

void MessagingHelper::handleNestedFiltersFromMessageFilter(QMessageFilter &filter)
{
    QMessageStore* store = QMessageStore::instance();

    QMessageFilterPrivate* pMFFilter = QMessageFilterPrivate::implementation(filter);
    if (pMFFilter->_filterList.count() > 0) {
        int filterListCount = pMFFilter->_filterList.count();
        for (int i=0; i < filterListCount; i++) {
            for (int j=0; j < pMFFilter->_filterList[i].count(); j++) {
                QMessageFilterPrivate* pMFFilter2 = QMessageFilterPrivate::implementation(pMFFilter->_filterList[i][j]);
                if (pMFFilter2->_field == QMessageFilterPrivate::ParentAccountIdFilter) {
                    QMessageAccountIdList accountIds = store->queryAccounts(*pMFFilter2->_accountFilter, QMessageAccountSortOrder(), 0, 0);
                    QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter2->_comparatorValue));
                    if (accountIds.count() > 0) {
                        pMFFilter->_filterList[i].removeAt(j);
                        if (cmp == QMessageDataComparator::Includes) {
                            for (int x = 0; x < accountIds.count(); x++) {
                                if (x == 0) {
                                    if (x+1 < accountIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[i]);
                                    }
                                    pMFFilter->_filterList[i].append(QMessageFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                    qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                                } else {
                                    if (x+1 < accountIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[pMFFilter->_filterList.count()-1]);
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-2].append(QMessageFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-2].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-2].end(), QMessageFilterPrivate::lessThan);
                                    } else {
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-1].append(QMessageFilter::byParentAccountId(accountIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-1].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-1].end(), QMessageFilterPrivate::lessThan);
                                    }
                                }
                            }
                        } else { // Excludes
                            for (int x = 0; x < accountIds.count(); x++) {
                                pMFFilter->_filterList[i].append(QMessageFilter::byParentAccountId(accountIds[x],QMessageDataComparator::NotEqual));
                            }
                            qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                        }
                    } else {
                        delete pMFFilter2->_accountFilter;
                        pMFFilter2->_accountFilter = 0;
                        pMFFilter2->_field = QMessageFilterPrivate::Id;
                        qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                    }
                } else if (pMFFilter2->_field == QMessageFilterPrivate::ParentFolderIdFilter) {
                    QMessageFolderIdList folderIds = store->queryFolders(*pMFFilter2->_folderFilter, QMessageFolderSortOrder(), 0, 0);
                    QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter2->_comparatorValue));
                    if (folderIds.count() > 0) {
                        pMFFilter->_filterList[i].removeAt(j);
                        if (cmp == QMessageDataComparator::Includes) {
                            for (int x = 0; x < folderIds.count(); x++) {
                                if (x == 0) {
                                    if (x+1 < folderIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[i]);
                                    }
                                    pMFFilter->_filterList[i].append(QMessageFilter::byParentFolderId(folderIds[x],QMessageDataComparator::Equal));
                                    qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                                } else {
                                    if (x+1 < folderIds.count()) {
                                        pMFFilter->_filterList.append(pMFFilter->_filterList[pMFFilter->_filterList.count()-1]);
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-2].append(QMessageFilter::byParentFolderId(folderIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-2].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-2].end(), QMessageFilterPrivate::lessThan);
                                    } else {
                                        pMFFilter->_filterList[pMFFilter->_filterList.count()-1].append(QMessageFilter::byParentFolderId(folderIds[x],QMessageDataComparator::Equal));
                                        qSort(pMFFilter->_filterList[pMFFilter->_filterList.count()-1].begin(), pMFFilter->_filterList[pMFFilter->_filterList.count()-1].end(), QMessageFilterPrivate::lessThan);
                                    }
                                }
                            }
                        } else { // Excludes
                            for (int x = 0; x < folderIds.count(); x++) {
                                pMFFilter->_filterList[i].append(QMessageFilter::byParentFolderId(folderIds[x],QMessageDataComparator::NotEqual));
                            }
                            qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                        }
                    } else {
                        delete pMFFilter2->_folderFilter;
                        pMFFilter2->_folderFilter = 0;
                        pMFFilter2->_field = QMessageFilterPrivate::Id;
                        qSort(pMFFilter->_filterList[i].begin(), pMFFilter->_filterList[i].end(), QMessageFilterPrivate::lessThan);
                    }
                } else {
                    break;
                }
            }
        }
    } else {
        if (pMFFilter->_field == QMessageFilterPrivate::ParentAccountIdFilter) {
            QMessageAccountIdList accountIds = store->queryAccounts(*pMFFilter->_accountFilter, QMessageAccountSortOrder(), 0, 0);
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter->_comparatorValue));
            if (accountIds.count() > 0) {
                for (int i=0; i < accountIds.count(); i++) {
                    if (i == 0) {
                        delete pMFFilter->_accountFilter;
                        pMFFilter->_accountFilter = 0;
                        pMFFilter->_field = QMessageFilterPrivate::ParentAccountId;
                        pMFFilter->_value = accountIds[0].toString();
                        pMFFilter->_comparatorType = QMessageFilterPrivate::Equality;
                        if (cmp == QMessageDataComparator::Includes) {
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::Equal);
                        } else { // Excludes
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::NotEqual);
                        }
                    } else {
                        if (cmp == QMessageDataComparator::Includes) {
                            filter |= QMessageFilter::byParentAccountId(accountIds[i],QMessageDataComparator::Equal);
                        } else { // Excludes
                            filter &= QMessageFilter::byParentAccountId(accountIds[i],QMessageDataComparator::NotEqual);
                        }
                    }
                }
            } else {
                delete pMFFilter->_accountFilter;
                pMFFilter->_accountFilter = 0;
                pMFFilter->_field = QMessageFilterPrivate::Id;
            }
        } else if (pMFFilter->_field == QMessageFilterPrivate::ParentFolderIdFilter) {
            QMessageFolderIdList folderIds = store->queryFolders(*pMFFilter->_folderFilter, QMessageFolderSortOrder(), 0, 0);
            QMessageDataComparator::InclusionComparator cmp(static_cast<QMessageDataComparator::InclusionComparator>(pMFFilter->_comparatorValue));
            if (folderIds.count() > 0) {
                for (int i=0; i < folderIds.count(); i++) {
                    if (i == 0) {
                        delete pMFFilter->_folderFilter;
                        pMFFilter->_folderFilter = 0;
                        pMFFilter->_field = QMessageFilterPrivate::ParentFolderId;
                        pMFFilter->_value = folderIds[0].toString();
                        pMFFilter->_comparatorType = QMessageFilterPrivate::Equality;
                        if (cmp == QMessageDataComparator::Includes) {
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::Equal);
                        } else { // Excludes
                            pMFFilter->_comparatorValue = static_cast<int>(QMessageDataComparator::NotEqual);
                        }
                    } else {
                        if (cmp == QMessageDataComparator::Includes) {
                            filter |= QMessageFilter::byParentFolderId(folderIds[i],QMessageDataComparator::Equal);
                        } else { // Excludes
                            filter &= QMessageFilter::byParentFolderId(folderIds[i],QMessageDataComparator::NotEqual);
                        }
                    }
                }
            } else {
                delete pMFFilter->_folderFilter;
                pMFFilter->_folderFilter = 0;
                pMFFilter->_field = QMessageFilterPrivate::Id;
            }
        }
    }
}

bool MessagingHelper::preFilter(QMessageFilter &filter, QMessage::Type type)
{
    QMessageFilterPrivate* pMFFilter = QMessageFilterPrivate::implementation(filter);

    QString prefix;
    if (type == QMessage::Email) {
#if defined(Q_OS_SYMBIAN)
    #ifdef FREESTYLEMAILUSED
        prefix = "FS_";
    #else
        prefix = "MTM_";
    #endif
#else // Q_WS_MAEMO_5
        prefix = "MO_";
#endif
    } else if (type == QMessage::Sms) {
#if defined(Q_OS_SYMBIAN)
        prefix = "MTM_";
#else // Q_WS_MAEMO_5
        prefix = "el";
#endif
    }

    return pMFFilter->preFilter(type, prefix);
}

void MessagingHelper::extractMIMEHeaderParts(const QByteArray &mimeHeaderString, QByteArray &mimeType,
                                             QByteArray &mimeSubType, QByteArray &charset)
{
    int index = mimeHeaderString.indexOf("/");
    if (index != -1) {
        mimeType = mimeHeaderString.left(index).trimmed();

        mimeSubType = mimeHeaderString.mid(index + 1).trimmed();
        index = mimeSubType.indexOf(";");
        if (index != -1) {
            QString remainder = mimeSubType.mid(index + 1);
            mimeSubType = mimeSubType.left(index).trimmed();

            QRegExp charsetPattern("charset=(\\S+)");
            index = charsetPattern.indexIn(remainder);
            if (index != -1) {
                charset = charsetPattern.cap(1).toLatin1().toUpper();
            }
        }
    }
}

MessageCache* MessageCache::instance()
{
    return messageCache();
}

MessageCache::MessageCache()
{
    m_messageCache.setMaxCost(maxMessageCacheSize);
}

MessageCache::~MessageCache()
{
}

bool MessageCache::insert(const QMessage &message)
{
    bool retVal;

    m_mutex.lock();
    QMessage *msg = new QMessage(message);
    retVal = m_messageCache.insert(message.id().toString(), msg);
    if (retVal == false) {
        delete msg;
    }
    m_mutex.unlock();

    return retVal;
}

bool MessageCache::update(const QMessage &message)
{
    bool retVal;

    m_mutex.lock();
    retVal = m_messageCache.remove(message.id().toString());
    if (retVal) {
        QMessage *msg = new QMessage(message);
        m_messageCache.insert(message.id().toString(), msg);
    }
    m_mutex.unlock();

    return retVal;
}

bool MessageCache::remove(const QMessageId &id)
{
    bool retVal;

    m_mutex.lock();
    retVal = m_messageCache.remove(id.toString());
    m_mutex.unlock();

    return retVal;
}

QMessage MessageCache::message(const QMessageId &id)
{
    QMessage message;

    m_mutex.lock();
    QMessage *msg = m_messageCache.object(id.toString());
    if (msg) {
        message = *msg;
    }
    m_mutex.unlock();

    return message;
}

bool MessageCache::contains(const QMessageId &id)
{
    bool retVal;

    m_mutex.lock();
    retVal = m_messageCache.contains(id.toString());
    m_mutex.unlock();

    return retVal;
}

bool MessageCache::isFull() const
{
    if (m_messageCache.size() >= maxMessageCacheSize) {
        return true;
    }

    return false;
}

bool MessageCache::insertObject(QMessage *message)
{
    bool retVal;

    m_mutex.lock();
    retVal = m_messageCache.insert(message->id().toString(), message);
    m_mutex.unlock();

    return retVal;
}

QMessage* MessageCache::messageObject(const QMessageId &id)
{
    return m_messageCache.object(id.toString());
}

QMessageIdList MessageCache::messageIds()
{
    QMessageIdList ids;

    QStringList keys = m_messageCache.keys();
    for (int i=0; i < keys.count(); i++) {
        ids.append(QMessageId(keys[i]));
    }

    return ids;
}

int MessageCache::count() const
{
    return m_messageCache.count();
}

void MessageCache::lock()
{
    m_mutex.lock();
}

void MessageCache::unlock()
{
    m_mutex.unlock();
}

void MessageCache::clear()
{
    m_mutex.lock();
    m_messageCache.clear();
    m_mutex.unlock();
}

QTM_END_NAMESPACE
