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

#include "qdeclarativeopenmetaobject_p.h"
#include "qmetaobjectbuilder_p.h"
#include <qdebug.h>

QTM_BEGIN_NAMESPACE

class QDeclarativeOpenMetaObjectPrivate
{
public:
    QDeclarativeOpenMetaObjectPrivate(QDeclarativeOpenMetaObject *_q)
        : q(_q), parent(0), mem(0){}

    int propertyOffset() const
    {
        return m_propertyOffset;
    }

    int signalOffset() const
    {
        return m_signalOffset;
    }

    int createProperty(const QByteArray &name, const QByteArray& type)
    {
        int id = mob.propertyCount();
        mob.addSignal("__" + QByteArray::number(id) + "()");
        mob.addProperty(name, type, id);
        qFree(mem);
        mem = mob.toMetaObject();

        static_cast<QMetaObject *>(q)->d = mem->d;

        return m_propertyOffset + id;
    }

    const char* name(int propId)
    {
        return mob.property(propId).name();
    }

    void init(const QMetaObject *metaObj)
    {
        if (mem)
            qFree(mem);

        mob.setSuperClass(metaObj);
        mob.setClassName(metaObj->className());
        mob.setFlags(QMetaObjectBuilder::DynamicMetaObject);

        mem = mob.toMetaObject();

        m_propertyOffset = mem->propertyOffset();
        m_signalOffset = mem->methodOffset();
    }

    QDeclarativeOpenMetaObject *q;
    QAbstractDynamicMetaObject *parent;
    QObject *object;

    int m_propertyOffset;
    int m_signalOffset;

    QMetaObjectBuilder mob;
    QMetaObject *mem;
};

class QDeclarativeMetaDataHelperObject : public QObject
{
public:
    inline QObjectData *data() { return d_ptr.data(); }
};

/*!
    \class QDeclarativeOpenMetaObject
    \internal
    \brief open meta object for accessing dynamic properties in qml
    \since Mobility 1.2
*/

void QDeclarativeOpenMetaObject::setMetaObject(const QMetaObject& metaObject)
{
    d->init(&metaObject);

    static_cast<QMetaObject *>(this)->d = d->mem->d;

    //tricky code to replace the object's metaObject
    static_cast<QDeclarativeMetaDataHelperObject *>(d->object)->data()->metaObject = this;
}

QDeclarativeOpenMetaObject::QDeclarativeOpenMetaObject(QObject *obj)
: d(new QDeclarativeOpenMetaObjectPrivate(this))
{
    d->object = obj;
    setMetaObject(*(obj->metaObject()));
}

QDeclarativeOpenMetaObject::~QDeclarativeOpenMetaObject()
{
    //tricky code to replace the object's metaObject
    static_cast<QDeclarativeMetaDataHelperObject *>(d->object)->data()->metaObject = 0;

    if (d->parent)
        delete d->parent;
    if (d->mem)
        qFree(d->mem);
    delete d;
}

int QDeclarativeOpenMetaObject::metaCall(QMetaObject::Call c, int id, void **a)
{
    if (( c == QMetaObject::ReadProperty || c == QMetaObject::WriteProperty)
            && id >= d->m_propertyOffset) {

        if (c == QMetaObject::ReadProperty) {
            propertyRead(id);
            getValue(id, a);
        } else if (c == QMetaObject::WriteProperty) {
            propertyWrite(id);
            setValue(id,a);
            propertyWritten(id);
            activate(d->object, d->m_signalOffset + id, 0);
        } 
        return -1;
    } else {
        if (d->parent)
            return d->parent->metaCall(c, id, a);
        else
            return d->object->qt_metacall(c, id, a);
    }
}

QAbstractDynamicMetaObject *QDeclarativeOpenMetaObject::parent() const
{
    return d->parent;
}

void QDeclarativeOpenMetaObject::getValue(int propId, void **a)
{
    Q_UNUSED(propId);
    Q_UNUSED(a);
}

void QDeclarativeOpenMetaObject::setValue(int propId, void **a)
{
    Q_UNUSED(propId);
    Q_UNUSED(a);
}


int QDeclarativeOpenMetaObject::createProperty(const char *name, const char *type)
{
    return d->createProperty(name, type);
}

void QDeclarativeOpenMetaObject::propertyRead(int)
{
}

void QDeclarativeOpenMetaObject::propertyWrite(int)
{
}

void QDeclarativeOpenMetaObject::propertyWritten(int)
{
}


QObject *QDeclarativeOpenMetaObject::object() const
{
    return d->object;
}

QTM_END_NAMESPACE
