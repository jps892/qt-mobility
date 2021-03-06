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
#include "qmessagefoldersortorder.h"
#include "qmessagefoldersortorder_p.h"

QTM_BEGIN_NAMESPACE

bool QMessageFolderSortOrderPrivate::lessThan(const QMessageFolderSortOrder &sortOrder,
                                             const QMessageFolder &folder1, const QMessageFolder &folder2)
{
    QMessageFolderSortOrderPrivate *d(sortOrder.d_ptr);
    
    QList<QPair<Field, Qt::SortOrder> >::iterator it(d->_fieldOrderList.begin());
    while (it != d->_fieldOrderList.end()) {
        Field field((*it).first);
        Qt::SortOrder order((*it).second);
        ++it;

        const QMessageFolder *left;
        const QMessageFolder *right;
        if (order == Qt::AscendingOrder) {
            left = &folder1;
            right = &folder2; 
        } else {
            left = &folder2;
            right = &folder1; 
        }

        switch (field)
        {
        case Name: {
            if (left->name() < right->name()) {
                return true;
            } else if (right->name() < left->name()) {
                return false;
            } else {
                continue;
            }
        }
        case Path: {
            if (left->path() < right->path()) {
                return true;
            } else if (right->path() < left->path()) {
                return false;
            } else {
                continue;
            }
        }
        } // switch
    }

    return false; // equality
}

QMessageFolderSortOrder::QMessageFolderSortOrder()
 : d_ptr(new QMessageFolderSortOrderPrivate(this))
{
}

QMessageFolderSortOrder::QMessageFolderSortOrder(const QMessageFolderSortOrder &other)
 : d_ptr(new QMessageFolderSortOrderPrivate(this))
{
    this->operator=(other);
}

QMessageFolderSortOrder::~QMessageFolderSortOrder()
{
    delete d_ptr;
    d_ptr = 0;
}

QMessageFolderSortOrder& QMessageFolderSortOrder::operator=(const QMessageFolderSortOrder& other)
{
    if (&other != this) {
        d_ptr->_fieldOrderList = other.d_ptr->_fieldOrderList;
    }

    return *this;
}

bool QMessageFolderSortOrder::isEmpty() const
{
    return d_ptr->_fieldOrderList.isEmpty();
}

bool QMessageFolderSortOrder::isSupported() const
{
    return true;
}

QMessageFolderSortOrder QMessageFolderSortOrder::operator+(const QMessageFolderSortOrder& other) const
{
    QMessageFolderSortOrder sum;
    sum.d_ptr->_fieldOrderList = d_ptr->_fieldOrderList + other.d_ptr->_fieldOrderList;
    return sum;
}

QMessageFolderSortOrder& QMessageFolderSortOrder::operator+=(const QMessageFolderSortOrder& other)
{
    if (&other == this) {
        return *this;
    }
    
    d_ptr->_fieldOrderList += other.d_ptr->_fieldOrderList;
    
    return *this;
}

bool QMessageFolderSortOrder::operator==(const QMessageFolderSortOrder& other) const
{
    return (d_ptr->_fieldOrderList == other.d_ptr->_fieldOrderList);
}

QMessageFolderSortOrder QMessageFolderSortOrder::byName(Qt::SortOrder order)
{
    QMessageFolderSortOrder result;
    QPair<QMessageFolderSortOrderPrivate::Field, Qt::SortOrder> fieldOrder(QMessageFolderSortOrderPrivate::Name, order);
    result.d_ptr->_fieldOrderList.append(fieldOrder);
    return result;
}

QMessageFolderSortOrder QMessageFolderSortOrder::byPath(Qt::SortOrder order)
{
    QMessageFolderSortOrder result;
    QPair<QMessageFolderSortOrderPrivate::Field, Qt::SortOrder> fieldOrder(QMessageFolderSortOrderPrivate::Path, order);
    result.d_ptr->_fieldOrderList.append(fieldOrder);
    return result;
}

QTM_END_NAMESPACE
