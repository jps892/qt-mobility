/****************************************************************************
 **
 ** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

//API
#include "qdocumentgallery.h"
#include "qabstractgallery_p.h"

#include "qgalleryitemrequest.h"
#include "qgalleryqueryrequest.h"
#include "qgalleryremoverequest.h"
#include "qgallerytyperequest.h"
//Qt
#include <QtCore/qmetaobject.h>

//Backend
#include "qgallerymdsutility.h"
#include "qmdegallerytyperesultset.h"
#include "qmdegalleryitemresultset.h"
#include "qmdegalleryremoveresultset.h"
#include "qmdegalleryqueryresultset.h"
#include "qmdesession.h"

QTM_BEGIN_NAMESPACE

class QDocumentGalleryPrivate : public QAbstractGalleryPrivate
{
public:
    QGalleryAbstractResponse* createTypeResponse(QGalleryTypeRequest *request);
    QGalleryAbstractResponse* createItemResponse(QGalleryItemRequest *request);
    QGalleryAbstractResponse* createRemoveResponse(QGalleryRemoveRequest *request);
    QGalleryAbstractResponse* createQueryResponse(QGalleryQueryRequest *request);

    QMdeSession *m_session;
};

QGalleryAbstractResponse* QDocumentGalleryPrivate::createTypeResponse(QGalleryTypeRequest *request)
{
    // Fill up response class with with request data (create proper query)
    QMDEGalleryTypeResultSet *response = new QMDEGalleryTypeResultSet(m_session, request);
    return response;

}

QGalleryAbstractResponse* QDocumentGalleryPrivate::createItemResponse(QGalleryItemRequest *request)
{
    // Fill up response class with with request data (create proper query)
    QMDEGalleryItemResultSet *response = new QMDEGalleryItemResultSet(m_session, request);
    return response;
}

QGalleryAbstractResponse* QDocumentGalleryPrivate::createRemoveResponse(QGalleryRemoveRequest *request)
{
    // Fill up response class with with request data (create proper query)
    QMDEGalleryRemoveResultSet *response = new QMDEGalleryRemoveResultSet(m_session, request);
    return response;

}

QGalleryAbstractResponse* QDocumentGalleryPrivate::createQueryResponse(QGalleryQueryRequest *request)
{
    // Fill up response class with with request data (create proper query)
    QMDEGalleryQueryResultSet *response = new QMDEGalleryQueryResultSet(m_session, request);
    return response;
}

QDocumentGallery::QDocumentGallery(QObject *parent)
: QAbstractGallery(*new QDocumentGalleryPrivate, parent)
{
    Q_D(QDocumentGallery); // access to private class
    d->m_session = new QMdeSession(this);
}

QDocumentGallery::~QDocumentGallery()
{

}

bool QDocumentGallery::isRequestSupported(QGalleryAbstractRequest::RequestType type) const
{
    switch (type) {
    case QGalleryAbstractRequest::QueryRequest:
    case QGalleryAbstractRequest::ItemRequest:
    case QGalleryAbstractRequest::TypeRequest:
    case QGalleryAbstractRequest::RemoveRequest:
        return true;
        default:
        return false;
    }
}

QStringList QDocumentGallery::itemTypePropertyNames(const QString &itemType) const
{
    QStringList list;
    QDocumentGalleryMDSUtility::GetDataFieldsForItemType( list, itemType );
    return list;
}

QGalleryProperty::Attributes QDocumentGallery::propertyAttributes(
    const QString &propertyName, const QString &/*itemType*/) const
{
    if( propertyName == QDocumentGallery::url.name() ) {
        return (QGalleryProperty::CanRead | QGalleryProperty::CanSort | QGalleryProperty::CanFilter );
    } else {
        return (QGalleryProperty::CanRead | QGalleryProperty::CanWrite | QGalleryProperty::CanSort | QGalleryProperty::CanFilter );
    }
}

QGalleryAbstractResponse* QDocumentGallery::createResponse(QGalleryAbstractRequest *request)
{
    Q_D(QDocumentGallery); // access to private class

    switch (request->type()) {
    case QGalleryAbstractRequest::ItemRequest:
        return d->createItemResponse(static_cast<QGalleryItemRequest *>(request));
    case QGalleryAbstractRequest::TypeRequest:
        return d->createTypeResponse(static_cast<QGalleryTypeRequest *>(request));
    case QGalleryAbstractRequest::RemoveRequest:
        return d->createRemoveResponse(static_cast<QGalleryRemoveRequest *>(request));
    case QGalleryAbstractRequest::QueryRequest:
        return d->createQueryResponse(static_cast<QGalleryQueryRequest *>(request));
    default:
        return 0;
    }
}

#include "moc_qdocumentgallery.cpp"

QTM_END_NAMESPACE