/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtDeclarative module of the Qt Toolkit.
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
#include <QtDeclarative/qdeclarativeinfo.h>
#include "qdeclarativecontactrelationship_p.h"
#include <QDebug>

/*!
   \qmlclass Relationship QDeclarativeContactRelationship
   \brief The Relationship element describes a one-to-one relationship
  between a locally-stored contact and another (possibly remote) contact.
  \since Mobility 1.1

   \ingroup qml-contacts

   This element is part of the \bold{QtMobility.contacts 1.1} module.

   \sa QContactRelationship
   \sa RelationshipModel
 */






QDeclarativeContactRelationship::QDeclarativeContactRelationship(QObject* parent)
    :QObject(parent)
{

}


/*!
  \qmlproperty int Relationship::first
  \since Mobility 1.1

  This property holds the id of the locally-stored contact which has a relationship of the given type with the second contact.
  */

QContactLocalId QDeclarativeContactRelationship::first() const
{
    return m_relationship.first().localId();
}

/*!
  \qmlproperty int Relationship::second
  \since Mobility 1.1

  This property holds the id of the contact with which the first contact has a relationship of the given type.
  */
QContactLocalId  QDeclarativeContactRelationship::second() const
{
    return m_relationship.second().localId();
}

/*!
  \qmlproperty string Relationship::type
  \since Mobility 1.1

  This property holds the type of relationship which the source contact has with the destination contacts.
  The value for this property can be one of:
  \list
  \o HasMember
  \o Aggregates
  \o IsSameAs
  \o HasAssistant
  \o HasManager
  \o HasSpouse
  \endlist
  or any other customized relationship type string.
  */

QVariant QDeclarativeContactRelationship::relationshipType() const
{
    return m_relationship.relationshipType();
}

void QDeclarativeContactRelationship::setFirst(QContactLocalId firstId)
{
    QContactId id;
    id.setLocalId(firstId);
    m_relationship.setFirst(id);
}

void QDeclarativeContactRelationship::setSecond(QContactLocalId secondId)
{
    QContactId id;
    id.setLocalId(secondId);
    m_relationship.setSecond(id);
}

void QDeclarativeContactRelationship::setRelationshipType(const QVariant& relationshipType)
{
    if (relationshipType.type() == QVariant::Double) {//numbers in qml are set to double, even it's integer
        switch (relationshipType.toInt()) {
        case QDeclarativeContactRelationship::HasMember:
            m_relationship.setRelationshipType(QContactRelationship::HasMember);
            break;
        case QDeclarativeContactRelationship::Aggregates:
            m_relationship.setRelationshipType(QContactRelationship::Aggregates);
            break;
        case QDeclarativeContactRelationship::IsSameAs:
            m_relationship.setRelationshipType(QContactRelationship::IsSameAs);
            break;
        case QDeclarativeContactRelationship::HasAssistant:
            m_relationship.setRelationshipType(QContactRelationship::HasAssistant);
            break;
        case QDeclarativeContactRelationship::HasManager:
            m_relationship.setRelationshipType(QContactRelationship::HasManager);
            break;
        case QDeclarativeContactRelationship::HasSpouse:
            m_relationship.setRelationshipType(QContactRelationship::HasSpouse);
            break;
        default:
            //unknown type
            qmlInfo(this) << tr("unknown relationship type:") << relationshipType;
            break;
        }
    } else {
        m_relationship.setRelationshipType(relationshipType.toString());
    }

}


QContactRelationship QDeclarativeContactRelationship::relationship() const
{
    return m_relationship;
}
void QDeclarativeContactRelationship::setRelationship(const QContactRelationship& relationship)
{
    m_relationship = relationship;
    emit valueChanged();
}
