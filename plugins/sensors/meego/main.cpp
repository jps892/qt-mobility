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

#include "meegoaccelerometer.h"
#include "meegoals.h"
#include "meegocompass.h"
#include "meegomagnetometer.h"
#include "meegoorientationsensor.h"
#include "meegoproximitysensor.h"
#include "meegorotationsensor.h"
#include "meegotapsensor.h"
#include "meegogyroscope.h"

#include <qsensorplugin.h>
#include <qsensorbackend.h>
#include <qsensormanager.h>
#include <QDebug>

class meegoSensorPlugin : public QObject, public QSensorPluginInterface, public QSensorBackendFactory
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QSensorPluginInterface)
public:
    void registerSensors()
    {
        QSensorManager::registerBackend(QAccelerometer::type, meegoaccelerometer::id, this);
        QSensorManager::registerBackend(QAmbientLightSensor::type, meegoals::id, this);
        QSensorManager::registerBackend(QCompass::type, meegocompass::id, this);
        QSensorManager::registerBackend(QMagnetometer::type, meegomagnetometer::id, this);
        QSensorManager::registerBackend(QOrientationSensor::type, meegoorientationsensor::id, this);
        QSensorManager::registerBackend(QProximitySensor::type, meegoproximitysensor::id, this);
        QSensorManager::registerBackend(QRotationSensor::type, meegorotationsensor::id, this);
        QSensorManager::registerBackend(QTapSensor::type, meegotapsensor::id, this);
        QSensorManager::registerBackend(QGyroscope::type, meegogyroscope::id, this);
        qDebug() << "Loaded the MeeGo sensor plugin";
    }

    QSensorBackend *createBackend(QSensor *sensor)
    {
        if (sensor->identifier() == meegoaccelerometer::id)
            return new meegoaccelerometer(sensor);
        else if (sensor->identifier() == meegoals::id)
            return new meegoals(sensor);
        else if (sensor->identifier() == meegocompass::id)
            return new meegocompass(sensor);
        else if (sensor->identifier() == meegomagnetometer::id)
            return new meegomagnetometer(sensor);
        else if (sensor->identifier() == meegoorientationsensor::id)
            return new meegoorientationsensor(sensor);
        else if (sensor->identifier() == meegoproximitysensor::id)
            return new meegoproximitysensor(sensor);
        else if (sensor->identifier() == meegorotationsensor::id)
            return new meegorotationsensor(sensor);
        else if (sensor->identifier() == meegotapsensor::id)
            return new meegotapsensor(sensor);
        else if (sensor->identifier() == meegogyroscope::id)
            return new meegogyroscope(sensor);
        return 0;
    }
};

Q_EXPORT_PLUGIN2(libsensors_meego, meegoSensorPlugin)

#include "main.moc"