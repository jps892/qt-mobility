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


#include "qorganizercollection.h"
#include "qorganizercollection_p.h"
#include "qorganizercollectionid.h"

#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
  \class QOrganizerCollection
  \brief The QOrganizerCollection class represents a collection of items in a manager.
  \inmodule QtOrganizer
  \ingroup organizer-main
  \since 1.1

  A collection has an id and optionally some metadata, and contains zero or more items.
  Each different manager will have different requirements before a collection may be saved
  in it.  Some managers do not allow collections to be saved at all, while others may require
  a collection to have some minimal amount of meta data defined in it prior to save.
  For example, most managers require a valid value for the \c QOrganizerCollection::KeyName
  meta data key to be set prior to save.

  Every QOrganizerItem is contained within a collection when stored in a manager.
  To save an item in a collection, the client should call \l QOrganizerItem::setCollectionId()
  on the item, passing in the id of the destination collection as the argument, and then
  save the item in the manager.  To move an item from one collection to another, the client
  must fetch the item from the manager, set the collection id in the item to the id of the
  collection to which the client wishes the item to be moved, and then resave the item in the
  manager.  That is, the collection which an item is part of is treated as a property of the
  item.
 */

/*!
  \variable QOrganizerCollection::KeyName
  The constant key value which identifies the name meta data of a collection.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerCollection::KeyName, "Name");

/*!
  \variable QOrganizerCollection::KeyDescription
  The constant key value which identifies the description meta data of a collection.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerCollection::KeyDescription, "Description");

/*!
  \variable QOrganizerCollection::KeyColor
  The constant key value which identifies the color meta data of a collection.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerCollection::KeyColor, "Color");

/*!
  \variable QOrganizerCollection::KeyImage
  The constant key value which identifies the image meta data of a collection.
 */
Q_DEFINE_LATIN1_CONSTANT(QOrganizerCollection::KeyImage, "Image");

/*!
  Constructs a new collection
 */
QOrganizerCollection::QOrganizerCollection()
    : d(new QOrganizerCollectionData)
{
}

/*!
  Cleans up any memory in use by the collection
 */
QOrganizerCollection::~QOrganizerCollection()
{
}

/*!
  Constructs a new copy of the \a other collection
  \since 1.1
 */
QOrganizerCollection::QOrganizerCollection(const QOrganizerCollection& other)
    : d(other.d)
{
}

/*!
  Assigns this collection to be equal to the \a other collection
  \since 1.1
 */
QOrganizerCollection& QOrganizerCollection::operator=(const QOrganizerCollection& other)
{
    this->d = other.d;
    return *this;
}

/*!
  Returns true if the id of the collection is the same as that of the \a other collection.
  Does not check that the metadata of the collections is equal.
  \since 1.1
 */
bool QOrganizerCollection::operator==(const QOrganizerCollection &other) const
{
    return d->m_id == other.d->m_id;
}

/*!
  \fn QOrganizerCollection::operator!=(const QOrganizerCollection &other) const

  Returns true if the id of the collection is not the same as that of the \a other collection.
  Does not check that the metadata of the collections is not equal.
  \since 1.1
 */

/*!
  Returns the complete id of the collection, which includes the manager uri and the manager id of the collection
  \since 1.1
 */
QOrganizerCollectionId QOrganizerCollection::id() const
{
    return d->m_id;
}

/*!
  Sets the id of the collection to \a id.
  If the id is set to a null (default-constructed) id, saving the collection will cause the manager to save the
  collection as a new collection (if it supports that operation).
  \since 1.1
 */
void QOrganizerCollection::setId(const QOrganizerCollectionId& id)
{
    d->m_id = id;
}

/*!
  Sets the meta data of the collection to \a metaData.
  Not all managers support arbitrary meta data for collections.  You can see whether the meta data
  is compatible with the manager by calling \l QOrganizerManager::compatibleCollection().
  Attempting to save a collection with unsupported meta data in a manager will cause an error
  in the operation.
  \since 1.1
 */
void QOrganizerCollection::setMetaData(const QVariantMap& metaData)
{
    d->m_metaData = metaData;
}

/*!
  Returns the meta data of the collection
  \since 1.1
 */
QVariantMap QOrganizerCollection::metaData() const
{
    return d->m_metaData;
}

/*!
  Sets the meta data of the collection for the given \a key to the given \a value.
  Not all managers support all of the standard meta data keys (see \l QOrganizerCollection),
  and some will support extra or even arbitrary keys.  Similarly, not all managers support
  all possible data types for the meta data \a value.
  Attempting to save a collection with unsupported meta data in a manager will cause an error
  in the operation.
  \since 1.1
 */
void QOrganizerCollection::setMetaData(const QString& key, const QVariant& value)
{
    d->m_metaData.insert(key, value);
}

/*!
  Returns the meta data of the collection for the given \a key
  \since 1.1
 */
QVariant QOrganizerCollection::metaData(const QString& key) const
{
    return d->m_metaData.value(key);
}

/*! Returns the hash value for \a key.
  \since 1.1
*/
uint qHash(const QOrganizerCollection &key)
{
    uint hash = qHash(key.id());
    QVariantMap metadata = key.metaData();
    QVariantMap::const_iterator it;
    for (it = metadata.constBegin(); it != metadata.constEnd(); ++it) {
        hash += qHash(it.key())
                + QT_PREPEND_NAMESPACE(qHash)(it.value().toString());
    }

    return hash;
}

#ifndef QT_NO_DEBUG_STREAM
/*!
  Streams the \a collection to the given debug stream \a dbg, and returns the stream.
  \since 1.1
 */
QDebug operator<<(QDebug dbg, const QOrganizerCollection& collection)
{
    dbg.nospace() << "QOrganizerCollection(id=" << collection.id();
    QVariantMap metadata = collection.metaData();
    QVariantMap::const_iterator it;
    for (it = metadata.constBegin(); it != metadata.constEnd(); ++it) {
        dbg.nospace() << ", " << it.key() << '=' << it.value();
    }
    dbg.nospace() << ')';
    return dbg.maybeSpace();
}
#endif

#ifndef QT_NO_DATASTREAM
/*!
 * Writes \a collection to the stream \a out.
  \since 1.1
 */
QDataStream& operator<<(QDataStream& out, const QOrganizerCollection& collection)
{
    quint8 formatVersion = 1; // Version of QDataStream format for QOrganizerCollection
    return out << formatVersion
               << collection.id().toString()
               << collection.metaData();
}

/*!
 * Reads an organizer collection from stream \a in into \a collection.
  \since 1.1
 */
QDataStream& operator>>(QDataStream& in, QOrganizerCollection& collection)
{
    quint8 formatVersion;
    in >> formatVersion;
    if (formatVersion == 1) {
        QString idString;
        QVariantMap metadata;
        in >> idString >> metadata;

        collection = QOrganizerCollection();
        collection.setId(QOrganizerCollectionId::fromString(idString));

        QMapIterator<QString, QVariant> it(metadata);
        while (it.hasNext()) {
            it.next();
            collection.setMetaData(it.key(), it.value());
        }
    } else {
        in.setStatus(QDataStream::ReadCorruptData);
    }
    return in;
}
#endif

QTM_END_NAMESPACE
