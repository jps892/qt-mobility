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
#ifndef QMESSAGEACCOUNTFILTERPRIVATE_H
#define QMESSAGEACCOUNTFILTERPRIVATE_H
#include "qmessageaccountfilter.h"
#ifdef Q_OS_WIN
#include "winhelpers_p.h"
#endif

#include <QSet>

#ifdef Q_OS_SYMBIAN
#include <qmessageaccount.h>
#include <QVariant>
#endif

QTM_BEGIN_NAMESPACE

class QMessageAccountFilterPrivate
{
    Q_DECLARE_PUBLIC(QMessageAccountFilter)

public:
#ifndef Q_OS_SYMBIAN
    enum Criterion { None = 0, IdEquality, IdInclusion, NameEquality, NameInclusion };
    enum Operator { Identity = 0, And, Or, Not, Nand, Nor, OperatorEnd };
#endif

    QMessageAccountFilterPrivate(QMessageAccountFilter *accountFilter);
    ~QMessageAccountFilterPrivate();
    QMessageAccountFilter *q_ptr;

#ifdef Q_OS_SYMBIAN
    typedef QList<QMessageAccountFilter> SortedMessageAccountFilterList;
    
    bool filter(QMessageAccount &messageAccount);

    static void applyNot(QMessageAccountFilter& filter);
    static bool lessThan(const QMessageAccountFilter filter1, const QMessageAccountFilter filter2);
    static QMessageAccountFilterPrivate* implementation(const QMessageAccountFilter &filter);

    QMessageDataComparator::Options _options;    
    
    enum Field {None = 0, Id, Name};
    enum Comparator {Equality = 0, Inclusion};
    
    bool _valid;
    
    QMessageAccountIdList _ids;
    QVariant _value;
    Field _field;

    Comparator _comparatorType;
    int _comparatorValue;
    
    QList<SortedMessageAccountFilterList> _filterList;
#endif

#ifdef Q_OS_WIN
    QMessageAccountFilterPrivate &operator=(const QMessageAccountFilterPrivate &other);
    bool operator==(const QMessageAccountFilterPrivate &other);
    static bool isNonMatching(const QMessageAccountFilter &filter);

    Operator _operator;
    Criterion _criterion;
    QSet<QMessageAccountId> _ids;
    QString _name;
    QMessageDataComparator::EqualityComparator _equality;
    QMessageDataComparator::InclusionComparator _inclusion;
    QMessageDataComparator::Options _options;
    bool _valid;
    QList<QMessageAccountFilter*> _arguments; // for bool ops

    static bool matchesStore(const QMessageAccountFilter &filter, const MapiStorePtr &store);
#endif
};

QTM_END_NAMESPACE
#endif