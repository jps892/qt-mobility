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
#include <QtGui>
#include <QtWebKit>

#include <qnmeapositioninfosource.h>

#include "mapwindow.h"

// Use the special 'localhost' key for the Google Maps key
const QString GMAPS_STATICMAP_URL_TEMPLATE =  "http://maps.google.com/staticmap?center=%1,%2&zoom=14&size=%3x%4&map type=mobile&markers=%1,%2&key=ABQIAAAAnfs7bKE82qgb3Zc2YyS-oBT2yXp_ZAY8_ufC3CFXhHIE1NvwkxSySz_REpPq-4WZA27OwgbtyR3VcA&sensor=false";


MapWindow::MapWindow(QWidget *parent, Qt::WFlags flags)
    : QMainWindow(parent, flags),
      source(new QNmeaPositionInfoSource(QNmeaPositionInfoSource::SimulationMode, this)),
      webView(new QWebView),
      posLabel(new QLabel),
      headingAndSpeedLabel(new QLabel),
      dateTimeLabel(new QLabel),
      loading(false)
{
    QFile *logFile = new QFile(QApplication::applicationDirPath()
            + QDir::separator() + "nmealog.txt", this);
    source->setDevice(logFile);
    source->setUpdateInterval(1500);
    connect(source, SIGNAL(positionUpdated(QGeoPositionInfo)),
            this, SLOT(positionUpdated(QGeoPositionInfo)));

    connect(webView, SIGNAL(loadStarted()), this, SLOT(loadStarted()));
    connect(webView, SIGNAL(loadFinished(bool)), this, SLOT(loadFinished(bool)));

    QWidget *mainWidget = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(mainWidget);
    layout->addWidget(webView);
    layout->addWidget(posLabel);
    layout->addWidget(headingAndSpeedLabel);
    layout->addWidget(dateTimeLabel);
    setCentralWidget(mainWidget);

    resize(300, 300);
    setWindowTitle(tr("Google Maps Demo"));
}

void MapWindow::start()
{
    source->startUpdates();
}

void MapWindow::positionUpdated(const QGeoPositionInfo &info)
{
    QString heading = "?";
    QString speed = "?";
    if (info.hasProperty(QGeoPositionInfo::Heading))
        heading = QString("%1%2").arg(info.property(QGeoPositionInfo::Heading)).arg(QChar(0x00b0));
    if (info.hasProperty(QGeoPositionInfo::GroundSpeed))
        speed = QString::number(info.property(QGeoPositionInfo::GroundSpeed) * 3.6, 'f', 1);
    posLabel->setText(tr("Position: %1").arg(info.coordinate().toString()));
    headingAndSpeedLabel->setText(tr("Bearing %1, travelling at %2 km/h").arg(heading).arg(speed));
    dateTimeLabel->setText(tr("(Last update: %1)").
            arg(info.dateTime().toLocalTime().time().toString()));

    if (!loading) {
        // Google Maps does not provide maps larger than 640x480
        int width = qMin(webView->width(), 640);
        int height = qMin(webView->height(), 480);
        QString url = GMAPS_STATICMAP_URL_TEMPLATE
                            .arg(QString::number(info.coordinate().latitude()))
                            .arg(QString::number(info.coordinate().longitude()))
                            .arg(QString::number(width))
                            .arg(QString::number(height));
        webView->load(url);
    }
}

void MapWindow::loadStarted()
{
    loading = true;
    webView->setUpdatesEnabled(false);
}

void MapWindow::loadFinished(bool)
{
    loading = false;
    webView->setUpdatesEnabled(true);
}