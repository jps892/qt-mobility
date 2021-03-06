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

#include "meegoaccelerometer.h"
#include "meegoals.h"
#include "meegocompass.h"
#include "meegomagnetometer.h"
#include "meegoorientationsensor.h"
#include "meegoproximitysensor.h"
#include "meegoirproximitysensor.h"
#include "meegorotationsensor.h"
#include "meegotapsensor.h"
#include "meegogyroscope.h"
#include "meegolightsensor.h"

#include <qsensorplugin.h>
#include <qsensorbackend.h>
#include <qsensormanager.h>
#include <QDebug>
#include <QSettings>

class meegoSensorPlugin : public QObject, public QSensorPluginInterface, public QSensorBackendFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QSensorPluginInterface)
    
public:
            
    void registerSensors()
    {
        // if no default - no support either, uses Sensors.conf
        QSettings settings(QSettings::SystemScope, QLatin1String("Nokia"), QLatin1String("Sensors"));
        settings.beginGroup(QLatin1String("Default"));
        QStringList keys = settings.allKeys();
        for (int i=0,l=keys.size(); i<l; i++){
            QString type = keys.at(i);
            QSensorManager::registerBackend(type.toAscii(), settings.value(type).toByteArray(), this);
        }
        qDebug() << "Loaded the MeeGo sensor plugin";
    }


    QSensorBackend *createBackend(QSensor *sensor)
    {
        if (sensor->identifier() == meegoaccelerometer::id)
            return new meegoaccelerometer(sensor);
        if (sensor->identifier() == meegoals::id)
            return new meegoals(sensor);
        if (sensor->identifier() == meegocompass::id)
            return new meegocompass(sensor);
        if (sensor->identifier() == meegomagnetometer::id)
            return new meegomagnetometer(sensor);
        if (sensor->identifier() == meegoorientationsensor::id)
            return new meegoorientationsensor(sensor);
        if (sensor->identifier() == meegoproximitysensor::id)
            return new meegoproximitysensor(sensor);
        if (sensor->identifier() == meegorotationsensor::id)
            return new meegorotationsensor(sensor);
        if (sensor->identifier() == meegotapsensor::id)
            return new meegotapsensor(sensor);
        if (sensor->identifier() == meegogyroscope::id)
            return new meegogyroscope(sensor);
        if (sensor->identifier() == meegolightsensor::id)
            return new meegolightsensor(sensor);
        if (sensor->identifier() == meegoirproximitysensor::id)
            return new meegoirproximitysensor(sensor);
        return 0;
    }
};

Q_EXPORT_PLUGIN2(libsensors_meego, meegoSensorPlugin)

#include "main.moc"
