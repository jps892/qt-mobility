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
#include "qmessagefolderfilter.h"
#include "qmfhelpers_p.h"

#include <qmailaccountkey.h>
#include <qmailfolderkey.h>
#include <qmailmessagekey.h>

using namespace QTM_PREPEND_NAMESPACE(QmfHelpers);

QTM_BEGIN_NAMESPACE

class QMessageFolderFilterPrivate
{
public:
    QMessageFolderFilterPrivate() : _key(), _matchFlags(0) {}

    QMailFolderKey _key;
    QMessageDataComparator::MatchFlags _matchFlags;

    //static QMessageFolderFilter convert(const QMailFolderKey &key);
    static QMailFolderKey convert(const QMessageFolderFilter &filter);
};

/*
QMessageFolderFilter QMessageFolderFilterPrivate::convert(const QMailFolderKey &key)
{
    QMessageFolderFilter result;
    result.d_ptr->_key = key;
    return result;
}
*/

QMailFolderKey QMessageFolderFilterPrivate::convert(const QMessageFolderFilter &filter)
{
    return filter.d_ptr->_key;
}

namespace QmfHelpers {

/*
QMessageFolderFilter convert(const QMailFolderKey &key)
{
    return QMessageFolderFilterPrivate::convert(key);
}
*/

QMailFolderKey convert(const QMessageFolderFilter &filter)
{
    return QMessageFolderFilterPrivate::convert(filter);
}

}

QMessageFolderFilter::QMessageFolderFilter()
    : d_ptr(new QMessageFolderFilterPrivate)
{
}

QMessageFolderFilter::QMessageFolderFilter(const QMessageFolderFilter &other)
    : d_ptr(new QMessageFolderFilterPrivate)
{
    this->operator=(other);
}

QMessageFolderFilter::~QMessageFolderFilter()
{
    delete d_ptr;
    d_ptr = 0;
}

QMessageFolderFilter& QMessageFolderFilter::operator=(const QMessageFolderFilter& other)
{
    if (&other != this) {
        d_ptr->_key = other.d_ptr->_key;
        d_ptr->_matchFlags = other.d_ptr->_matchFlags;
    }

    return *this;
}

void QMessageFolderFilter::setMatchFlags(QMessageDataComparator::MatchFlags matchFlags)
{
    d_ptr->_matchFlags = matchFlags;
}

QMessageDataComparator::MatchFlags QMessageFolderFilter::matchFlags() const
{
    return d_ptr->_matchFlags;
}

bool QMessageFolderFilter::isEmpty() const
{
    return d_ptr->_key.isEmpty();
}

bool QMessageFolderFilter::isSupported() const
{
    return !d_ptr->_matchFlags;
}

QMessageFolderFilter QMessageFolderFilter::operator~() const
{
    QMessageFolderFilter result;
    result.d_ptr->_key = ~d_ptr->_key;
    return result;
}

QMessageFolderFilter QMessageFolderFilter::operator&(const QMessageFolderFilter& other) const
{
    QMessageFolderFilter result;
    result.d_ptr->_key = d_ptr->_key & other.d_ptr->_key;
    result.d_ptr->_matchFlags = d_ptr->_matchFlags | other.d_ptr->_matchFlags; // matchFlags not supported
    return result;
}

QMessageFolderFilter QMessageFolderFilter::operator|(const QMessageFolderFilter& other) const
{
    QMessageFolderFilter result;
    result.d_ptr->_key = d_ptr->_key | other.d_ptr->_key;
    result.d_ptr->_matchFlags = d_ptr->_matchFlags | other.d_ptr->_matchFlags; // matchFlags not supported
    return result;
}

const QMessageFolderFilter& QMessageFolderFilter::operator&=(const QMessageFolderFilter& other)
{
    d_ptr->_key &= other.d_ptr->_key;
    d_ptr->_matchFlags |= other.d_ptr->_matchFlags; // matchFlags not supported
    return *this;
}

const QMessageFolderFilter& QMessageFolderFilter::operator|=(const QMessageFolderFilter& other)
{
    d_ptr->_key |= other.d_ptr->_key;
    d_ptr->_matchFlags |= other.d_ptr->_matchFlags; // matchFlags not supported
    return *this;
}

bool QMessageFolderFilter::operator==(const QMessageFolderFilter& other) const
{
    return ((d_ptr->_key == other.d_ptr->_key)
            && (d_ptr->_matchFlags == other.d_ptr->_matchFlags));
}

QMessageFolderFilter QMessageFolderFilter::byId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_key = QMailFolderKey::id(convert(id), convert(cmp));
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byId(const QMessageFolderIdList &ids, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_key = QMailFolderKey::id(convert(ids), convert(cmp));
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byId(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_key = QMailFolderKey::id(convert(filter), convert(cmp));
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byName(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_key = QMailFolderKey::displayName(value, convert(cmp));
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byName(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_key = QMailFolderKey::displayName(value, convert(cmp));
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byPath(const QString &value, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_key = QMailFolderKey::path(value, convert(cmp));
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byPath(const QString &value, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_key = QMailFolderKey::path(value, convert(cmp));
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byParentAccountId(const QMessageAccountId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_key = QMailFolderKey::parentAccountId(convert(id), convert(cmp));
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byParentAccountId(const QMessageAccountFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_key = QMailFolderKey::parentAccountId(convert(filter), convert(cmp));
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byParentFolderId(const QMessageFolderId &id, QMessageDataComparator::EqualityComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_key = QMailFolderKey::parentFolderId(convert(id), convert(cmp));
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byParentFolderId(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_key = QMailFolderKey::parentFolderId(convert(filter), convert(cmp));
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byAncestorFolderIds(const QMessageFolderId &id, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_key = QMailFolderKey::ancestorFolderIds(convert(id), convert(cmp));
    return result;
}

QMessageFolderFilter QMessageFolderFilter::byAncestorFolderIds(const QMessageFolderFilter &filter, QMessageDataComparator::InclusionComparator cmp)
{
    QMessageFolderFilter result;
    result.d_ptr->_key = QMailFolderKey::ancestorFolderIds(convert(filter), convert(cmp));
    return result;
}

QTM_END_NAMESPACE
