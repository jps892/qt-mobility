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

#ifndef UT_QVERSITCONTACTEXPORTER_H
#define UT_QVERSITCONTACTEXPORTER_H

#include <qcontactdetail.h>
#include <QObject>
#include <qmobilityglobal.h>

QTM_BEGIN_NAMESPACE

class QVersitContactExporter;
class MyQVersitContactExporterPrivate;

QTM_END_NAMESPACE
QTM_USE_NAMESPACE

class UT_QVersitContactExporter : public QObject
{
    Q_OBJECT

public slots:
    void scale(const QString& imageFileName, QByteArray& imageData);

private slots:
    void init();
    void cleanup();
    void initTestCase();
    void cleanupTestCase();

    void testConvertContact();
    void testUnknownContactDetails();
    void testEncodeName();
    void testEncodePhoneNumber();
    void testEncodeEmailAddress();
    void testEncodeStreetAddress();
    void testEncodeUrl();
    void testEncodeParameters();
    void testEncodeUid();
    void testEncodeRev();
    void testEncodeBirthDay();
    void testEncodeNote();
    void testEncodeGeoLocation();
    void testEncodeOrganization();
    void testEncodeEmbeddedContent();
    void testIsValidRemoteUrl();
    void testEncodeGender();
    void testEncodeNickName();
    void testEncodeAnniversary();
    void testEncodeOnlineAccount();
    void testEncodeFamily();
    void testEncodeAvatar();
    void testEncodeDisplayLabel();

    // Test Utility Function
    QContactDetail searchDetail(QList<QContactDetail> details, QString search);

private: // Data
    QVersitContactExporter* mExporter;
    MyQVersitContactExporterPrivate* mExporterPrivate;
    QString mTestPhotoFile;
    QString mTestAudioFile;
    bool mScaleSignalEmitted;
    QByteArray mSimulatedImageData;
};

#endif // UT_QVERSITCONTACTEXPORTER_H