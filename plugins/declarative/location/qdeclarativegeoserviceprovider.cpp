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

#include "qdeclarativegeoserviceprovider_p.h"

#include <QDebug>

QTM_BEGIN_NAMESPACE

/*!
    \qmlclass Plugin

    \brief The Plugin element describes a Location based services plugin.
    \inherits QObject

    \ingroup qml-location-maps
    \since Mobility 1.2

    An example of setting the Plugin in the Map element:
    \snippet examples/declarative-location/landmarkmap/landmarkmap.qml Map Plugin

    The Plugin element is part of the \bold{QtMobility.location 1.2} module.
*/

QDeclarativeGeoServiceProvider::QDeclarativeGeoServiceProvider(QObject *parent)
    : QObject(parent) {}

QDeclarativeGeoServiceProvider::~QDeclarativeGeoServiceProvider() {}

/*!
    \qmlproperty string Plugin::name
    \since Mobility 1.2

    This property holds the name of the plugin.
*/
void QDeclarativeGeoServiceProvider::setName(const QString &name)
{
    if (name_ == name)
        return;

    name_ = name;

    emit nameChanged(name_);
}

QString QDeclarativeGeoServiceProvider::name() const
{
    return name_;
}

/*!
    \qmlproperty list<PluginParameter> Plugin::parameters
    \default
    \since Mobility 1.2

    This property holds the list of plugin parameters.
*/
QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> QDeclarativeGeoServiceProvider::parameters()
{
    return QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter>(this,
            0,
            parameter_append,
            parameter_count,
            parameter_at,
            parameter_clear);
}

void QDeclarativeGeoServiceProvider::parameter_append(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> *prop, QDeclarativeGeoServiceProviderParameter *parameter)
{
    static_cast<QDeclarativeGeoServiceProvider*>(prop->object)->parameters_.append(parameter);
}

int QDeclarativeGeoServiceProvider::parameter_count(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> *prop)
{
    return static_cast<QDeclarativeGeoServiceProvider*>(prop->object)->parameters_.count();
}

QDeclarativeGeoServiceProviderParameter* QDeclarativeGeoServiceProvider::parameter_at(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> *prop, int index)
{
    return static_cast<QDeclarativeGeoServiceProvider*>(prop->object)->parameters_[index];
}

void QDeclarativeGeoServiceProvider::parameter_clear(QDeclarativeListProperty<QDeclarativeGeoServiceProviderParameter> *prop)
{
    static_cast<QDeclarativeGeoServiceProvider*>(prop->object)->parameters_.clear();
}

QMap<QString, QVariant> QDeclarativeGeoServiceProvider::parameterMap() const
{
    QMap<QString, QVariant> map;

    for(int i = 0; i < parameters_.size(); ++i) {
        QDeclarativeGeoServiceProviderParameter *parameter = parameters_.at(i);
        map.insert(parameter->name(), parameter->value());
    }

    return map;
}
/*******************************************************************************
*******************************************************************************/

/*!
    \qmlclass PluginParameter

    \brief The PluginParameter element describes the parameter to a \l Plugin.
    \inherits QObject

    \ingroup qml-location-maps
    \since Mobility 1.2

    The PluginParameter element is part of the \bold{QtMobility.location 1.2} module.
*/

QDeclarativeGeoServiceProviderParameter::QDeclarativeGeoServiceProviderParameter(QObject *parent)
    : QObject(parent) {}

QDeclarativeGeoServiceProviderParameter::~QDeclarativeGeoServiceProviderParameter() {}

/*!
    \qmlproperty string PluginParameter::name
    \since Mobility 1.2

    This property holds the name of the plugin parameter.
*/
void QDeclarativeGeoServiceProviderParameter::setName(const QString &name)
{
    if (name_ == name)
        return;

    name_ = name;

    emit nameChanged(name_);
}

QString QDeclarativeGeoServiceProviderParameter::name() const
{
    return name_;
}

/*!
    \qmlproperty QVariant PluginParameter::value
    \since Mobility 1.2

    This property holds the value of the plugin parameter.
*/
void QDeclarativeGeoServiceProviderParameter::setValue(const QVariant &value)
{
    if (value_ == value)
        return;

    value_ = value;

    emit valueChanged(value_);
}

QVariant QDeclarativeGeoServiceProviderParameter::value() const
{
    return value_;
}

/*******************************************************************************
*******************************************************************************/

#include "moc_qdeclarativegeoserviceprovider_p.cpp"

QTM_END_NAMESPACE

