/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
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
//system includes
#include <e32base.h>
#include <s32mem.h>
#include <cntitem.h>

//user includes
#include "cntsymbiandatabase.h"
#include "cntsymbiantransformerror.h"
#include "qcontactchangeset.h"
#include "qcontactmanagerengine.h"

// Constant
typedef QPair<QContactLocalId, QContactLocalId> QOwnCardPair;

CntSymbianDatabase::CntSymbianDatabase(QContactManagerEngine *engine, QContactManager::Error* error) :
    m_engine(engine),
    m_contactDatabase(0),
    m_currentOwnCardId(0),
    m_restoreStarted(false)
{
    TRAPD(err, initializeL());
    CntSymbianTransformError::transformError(err, error);
}

void CntSymbianDatabase::initializeL()
{
    User::LeaveIfNull(m_engine);

#ifdef SYMBIAN_BACKEND_USE_CNTMODEL_V2
    // 10.x platforms do not need some of CContactDatabase's concepts, so
    // they use the optimized OpenV2 and CreateV2.
    TRAPD(err, m_contactDatabase = CContactDatabase::OpenV2L());
    // Database not found, create it
    if (err == KErrNotFound) {
        m_contactDatabase = CContactDatabase::CreateV2L();
    } else {
        User::LeaveIfError(err);
    }
#else
    TRAPD(err, m_contactDatabase = CContactDatabase::OpenL());

    // Database not found, create it
    if (err == KErrNotFound) {
        m_contactDatabase = CContactDatabase::CreateL();
    } else {
        User::LeaveIfError(err);
    }
#endif

#ifndef SYMBIAN_BACKEND_USE_CNTMODEL_V2
    // In pre 10.1 platforms the AddObserverL & RemoveObserver functions are not
    // exported so we need to use CContactChangeNotifier.
    TRAP(err, m_contactChangeNotifier = CContactChangeNotifier::NewL(*m_contactDatabase, this));
#else
    TRAP(err, m_contactDatabase->AddObserverL(*this));
    TRAP(err, m_contactDatabase->AddObserverV2L(*this));
#endif

    // Read current own card id (self contact id)
    TContactItemId myCard = m_contactDatabase->OwnCardId();
    if (myCard > 0)
        m_currentOwnCardId = QContactLocalId(myCard);

    // Currently the group membership check is only used in pre-10.1
    // platforms. In 10.1 we need to check the performance penalty
    // caused in the instantiation of QContactManager. If the
    // performance is too bad, then the MContactDbObserver API needs to
    // be changed in 10.1 so that we don't need the group membership
    // buffer in the engine level. In other words events like
    // EContactDbObserverEventGroupMembersAdded and 
    // EContactDbObserverEventGroupMembersRemoved need to be added to
    // MContactDbObserver.
#ifndef SYMBIAN_BACKEND_USE_CNTMODEL_V2
    updateGroupMembershipsL();
#endif
}

CntSymbianDatabase::~CntSymbianDatabase()
{
    m_engine = NULL;
#ifndef SYMBIAN_BACKEND_USE_CNTMODEL_V2
    delete m_contactChangeNotifier;
#else
    if (m_contactDatabase != 0) {
        m_contactDatabase->RemoveObserver(*this);
        m_contactDatabase->RemoveObserverV2(*this);
    }
#endif
    delete m_contactDatabase;
}

CContactDatabase* CntSymbianDatabase::contactDatabase()
{
    return m_contactDatabase;
}

void CntSymbianDatabase::appendContactsEmitted(const QList<QContactLocalId> &contactList)
{
    m_contactsEmitted += contactList;
}

void CntSymbianDatabase::appendContactEmitted(QContactLocalId id)
{
    m_contactsEmitted.append(id);
}

/*!
 * Respond to a contacts database event, delegating this event to
 * an appropriate signal as required.
 *
 * \param aEvent Contacts database event describing the change to the
 *  database.
 */
void CntSymbianDatabase::HandleDatabaseEventL(TContactDbObserverEvent aEvent)
{
    QContactChangeSet changeSet;
    TContactItemId id = aEvent.iContactId;

#ifdef SYMBIAN_BACKEND_SIGNAL_EMISSION_TWEAK
    switch (aEvent.iType)
    {
    case EContactDbObserverEventContactAdded:
        if(m_contactsEmitted.contains(id))
            m_contactsEmitted.removeOne(id);
        else
            changeSet.insertAddedContact(id);
        break;
    case EContactDbObserverEventOwnCardDeleted:
        if (m_contactsEmitted.contains(id)) {
            m_contactsEmitted.removeOne(id);
        } else {
            // signal selfContactIdChanged (from id to zero)
            QOwnCardPair ownCard(m_currentOwnCardId, QContactLocalId(0));
            changeSet.setOldAndNewSelfContactId(ownCard);
            // signal contactsRemoved (the self contact was deleted)
            changeSet.insertRemovedContact(id);
        }
        // reset own card id
        m_currentOwnCardId = QContactLocalId(0);
        break;
    case EContactDbObserverEventContactDeleted:
        if(m_contactsEmitted.contains(id))
            m_contactsEmitted.removeOne(id);
        else
            changeSet.insertRemovedContact(id);
        break;
    case EContactDbObserverEventContactChanged:
        if(m_contactsEmitted.contains(id))
            m_contactsEmitted.removeOne(id);
        else
            changeSet.insertChangedContact(id);
        break;
    case EContactDbObserverEventGroupAdded:
        if(m_contactsEmitted.contains(id)) {
            // adding a group triggers also a "changed" event. The work-around
            // is to leave the id to m_contactsEmitted
        } else {
            changeSet.insertAddedContact(id);
            m_contactsEmitted.append(id);
        }
        break;
    case EContactDbObserverEventGroupDeleted:
        if(m_contactsEmitted.contains(id))
            m_contactsEmitted.removeOne(id);
        else
            changeSet.insertRemovedContact(id);
        break;
    case EContactDbObserverEventGroupChanged:
				//handled in HandleDatabaseEventV2L
        break;
    case EContactDbObserverEventOwnCardChanged:
        if (m_contactsEmitted.contains(id)) {
            m_contactsEmitted.removeOne(id);
        }
        else {
            if (m_currentOwnCardId == QContactLocalId(id)) {
                //own card content was changed
                changeSet.insertChangedContact(m_currentOwnCardId);
            }
        
            QOwnCardPair ownCard(m_currentOwnCardId, QContactLocalId(id));
            changeSet.setOldAndNewSelfContactId(ownCard);
            m_currentOwnCardId = QContactLocalId(id);
        }
        break;
    case EContactDbObserverEventRestoreBeginning:    
        m_restoreStarted = true;
        break;
    case EContactDbObserverEventBackupRestoreCompleted:    
        if (m_restoreStarted) {
            changeSet.setDataChanged(true);
            m_restoreStarted = false;
        }
        break;
    default:
        break; // ignore other events
    }
#else // SYMBIAN_BACKEND_SIGNAL_EMISSION_TWEAK
    switch (aEvent.iType)
    {
    case EContactDbObserverEventContactAdded:
        changeSet.insertAddedContact(id);
        break;
    case EContactDbObserverEventOwnCardDeleted:
        {
            // signal selfContactIdChanged (from id to zero)
            QOwnCardPair ownCard(m_currentOwnCardId, QContactLocalId(0));
            changeSet.setOldAndNewSelfContactId(ownCard);
            // signal contactsRemoved (the self contact was deleted)
            changeSet.insertRemovedContact(id);
            // reset own card id
            m_currentOwnCardId = QContactLocalId(0);
        }
        break;
    case EContactDbObserverEventContactDeleted:
        {
            changeSet.insertRemovedContact(id);

            // Check if contact was part of some group. 
            // This check is needed because CContactDatabase will NOT
            // provide EContactDbObserverEventGroupChanged event in this case!!!
            QMap<QContactLocalId, QSet<QContactLocalId> >::iterator i;
            for (i=m_groupContents.begin(); i!=m_groupContents.end(); i++ ) {
                if (i->contains(id)) {
                    changeSet.insertRemovedRelationshipsContact(i.key());
                    changeSet.insertRemovedRelationshipsContacts(i->toList());
                    i->remove(id);
                }
            }
        }
        break;
    case EContactDbObserverEventContactChanged:
        changeSet.insertChangedContact(id);
        break;
    case EContactDbObserverEventGroupAdded:
        // Creating a group will cause two events.
        // Emitting addedContact from EContactDbObserverEventGroupChanged.
        changeSet.insertAddedContact(id);
        break;
    case EContactDbObserverEventGroupDeleted:
        {
            changeSet.insertRemovedContact(id);
            
            // Check if there was any contacts in the group
            if (m_groupContents.value(id).count()) {
                changeSet.insertRemovedRelationshipsContact(id);
                changeSet.insertRemovedRelationshipsContacts(m_groupContents.value(id).toList());
            }
            m_groupContents.remove(id);
        }
        break;
    case EContactDbObserverEventGroupChanged:
        {
            bool isOldGroup = m_groupContents.contains(id);

            // Contact DB observer API does not give information of contacts
            // possibly added to or removed from the group
            QSet<QContactLocalId> added;
            QSet<QContactLocalId> removed;
            TRAPD(err, updateGroupMembershipsL(id, added, removed));        
            if(err != KErrNone)
                changeSet.setDataChanged(true);

            if (removed.count()) {
                // The group changed event was caused by removing contacts
                // from the group
                changeSet.insertRemovedRelationshipsContact(id);
                changeSet.insertRemovedRelationshipsContacts(removed.toList());
            }
            if (added.count()) {
                // The group changed event was caused by adding contacts
                // to the group
                changeSet.insertAddedRelationshipsContact(id);
                changeSet.insertAddedRelationshipsContacts(added.toList());
            }
            if (added.count() == 0 && removed.count() == 0) {
                // The group changed event was caused by modifying the group
                // NOTE: Do not emit this for a new group. Creating a group
                // through the backend causes two events GroupAdded and 
                // GroupChanged.
                if (isOldGroup)
                    changeSet.insertChangedContact(id);
            }
        }
        break;
    case EContactDbObserverEventOwnCardChanged:
        if (m_currentOwnCardId == QContactLocalId(id))
            changeSet.insertChangedContact(m_currentOwnCardId);
        else
            changeSet.setOldAndNewSelfContactId(QOwnCardPair(m_currentOwnCardId, QContactLocalId(id)));
        m_currentOwnCardId = QContactLocalId(id);
        break;
    case EContactDbObserverEventRestoreBeginning:    
        m_restoreStarted = true;
        break;
    case EContactDbObserverEventBackupRestoreCompleted:    
        if (m_restoreStarted) {
            changeSet.setDataChanged(true);
            m_restoreStarted = false;
        }
        break;        
    default:
        break; // ignore other events
    }
#endif // SYMBIAN_BACKEND_SIGNAL_EMISSION_TWEAK
    
    changeSet.emitSignals(m_engine);
}

#ifdef SYMBIAN_BACKEND_USE_CNTMODEL_V2  
/*!
 * Respond to a contacts database extended event, delegating this event to
 * an appropriate signal as required.
 *
 * \param aEvent Contacts database extended event describing the change to the
 *  database.
 */
void CntSymbianDatabase::HandleDatabaseEventV2L(TContactDbObserverEventV2 aEvent)
{
    QContactChangeSet changeSet;
    switch (aEvent.iTypeV2)
    {
    case EContactDbObserverEventV2ContactsAddedToGroup:
        if (aEvent.iAdditionalContactIds != NULL) {
            if (m_contactsEmitted.contains(aEvent.iContactId))
                {
                // Group id is in the list of emitted events, so remove 
                // group id and all affected contacts from this list.
                m_contactsEmitted.removeOne(aEvent.iContactId);
                for (int i = 0; i < aEvent.iAdditionalContactIds->Count(); i++) {
                    int id = aEvent.iAdditionalContactIds->operator[](i);
                    m_contactsEmitted.removeOne(id);
                    }
                }
            else {
                // No signal was sent yet, update changeSet
                QList<QContactLocalId> affectedContactIds;
                affectedContactIds.append(aEvent.iContactId);
                for (int i = 0; i < aEvent.iAdditionalContactIds->Count(); i++) {
                    int id = aEvent.iAdditionalContactIds->operator[](i);
                    affectedContactIds.append(id);
                }
                changeSet.insertAddedRelationshipsContacts(affectedContactIds);
            }
        }
        break;
    case EContactDbObserverEventV2ContactsRemovedFromGroup:
        if (aEvent.iAdditionalContactIds != NULL) {
            if (m_contactsEmitted.contains(aEvent.iContactId))
                {
                // Group id is in the list of emitted events, so remove 
                // group id and all affected contacts from this list.
                m_contactsEmitted.removeOne(aEvent.iContactId);
                for (int i = 0; i < aEvent.iAdditionalContactIds->Count(); i++) {
                    int id = aEvent.iAdditionalContactIds->operator[](i);
                    m_contactsEmitted.removeOne(id);
                    }
                }
            else {
                // No signal was sent yet, update changeSet
                QList<QContactLocalId> affectedContactIds;
                affectedContactIds.append(aEvent.iContactId);
                for (int i = 0; i < aEvent.iAdditionalContactIds->Count(); i++) {
                    int id = aEvent.iAdditionalContactIds->operator[](i);
                    affectedContactIds.append(id);
                }
                changeSet.insertRemovedRelationshipsContacts(affectedContactIds);
            }
        }
        break;
    case EContactDbObserverEventV2GroupChanged:
        if(m_contactsEmitted.contains(aEvent.iContactId)) {
            m_contactsEmitted.removeOne(aEvent.iContactId);
        }
        else {
            changeSet.insertChangedContact(aEvent.iContactId);
        }
        break;
    default:
        break; // ignore other events   
    }
    changeSet.emitSignals(m_engine);
}
#endif

/*
 * Private implementation for updating the buffer containing the members of all
 * groups.
 */
void CntSymbianDatabase::updateGroupMembershipsL()
{
    CContactIdArray *groupIds = m_contactDatabase->GetGroupIdListL();
    for (TInt i(0); i < groupIds->Count(); ++i) {
        QContactLocalId id = (*groupIds)[i];
        QSet<QContactLocalId> dummySet;
        updateGroupMembershipsL(id, dummySet, dummySet);
    }
    delete groupIds;
}

/*
 * Private implementation for updating the buffer containing the members of a
 * group.
 */
void CntSymbianDatabase::updateGroupMembershipsL(
    QContactLocalId groupId,
    QSet<QContactLocalId> &added,
    QSet<QContactLocalId> &removed)
{
    QSet<QContactLocalId> groupMembersNew = groupMembersL(groupId);
    QSet<QContactLocalId> groupMembersOld = m_groupContents.value(groupId);

    if(groupMembersOld.count() < groupMembersNew.count())
        added = groupMembersNew - groupMembersOld;
    else if(groupMembersOld.count() > groupMembersNew.count())
        removed = groupMembersOld - groupMembersNew;

    m_groupContents.insert(groupId, groupMembersNew);
}

/*
 * Private implementation for fetching the members of a group.
 */
QSet<QContactLocalId> CntSymbianDatabase::groupMembersL(QContactLocalId groupId)
{
    QSet<QContactLocalId> groupMembers;

    CContactItem *contactItem = m_contactDatabase->ReadContactLC(TContactItemId(groupId));
    Q_ASSERT(contactItem && contactItem->Type() == KUidContactGroup);
    CContactGroup *group = static_cast<CContactGroup*>(contactItem);
    
    const CContactIdArray *idArray = group->ItemsContained();
    
    //loop through all the contacts and add them to the list
    for (int i(0); i < idArray->Count(); i++) {
        groupMembers.insert((*idArray)[i]);
    }
    CleanupStack::PopAndDestroy(contactItem);

    return groupMembers;
}
