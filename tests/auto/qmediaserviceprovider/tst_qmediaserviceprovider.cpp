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

#include <QtTest/QtTest>
#include <QDebug>

#include <qmediaserviceprovider.h>
#include <qmediaserviceproviderplugin.h>
#include <qmediapluginloader_p.h>
#include <qmediaobject.h>
#include <qmediaservice.h>
#include <qmediaplayer.h>
#include <experimental/qcamera.h>
#include <qaudiocapturesource.h>

QTM_USE_NAMESPACE
class MockMediaService : public QMediaService
{
    Q_OBJECT
public:
    MockMediaService(const QString& name, QObject *parent = 0) : QMediaService(parent)
    { setObjectName(name); }
    ~MockMediaService() {}

    QMediaControl* control(const char *) const {return 0;}
};

class MockServicePlugin1 : public QMediaServiceProviderPlugin,
                           public QMediaServiceSupportedFormatsInterface,
                           public QMediaServiceSupportedDevicesInterface
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QMediaServiceSupportedFormatsInterface)
    Q_INTERFACES(QtMobility::QMediaServiceSupportedDevicesInterface)
public:
    QStringList keys() const
    {
        return QStringList() <<
                QLatin1String(Q_MEDIASERVICE_MEDIAPLAYER) <<
                QLatin1String(Q_MEDIASERVICE_CAMERA);
    }

    QMediaService* create(QString const& key)
    {
        if (keys().contains(key))
            return new MockMediaService("MockServicePlugin1");
        else
            return 0;
    }

    void release(QMediaService *service)
    {
        delete service;
    }

    QtMedia::SupportEstimate hasSupport(const QString &mimeType, const QStringList& codecs) const
    {        
        if (codecs.contains(QLatin1String("mpeg4")))
            return QtMedia::NotSupported;

        if (mimeType == "audio/ogg") {            
            return QtMedia::ProbablySupported;
        }

        return QtMedia::MaybeSupported;
    }

    QList<QByteArray> devices(const QByteArray &service) const
    {
        QList<QByteArray> res;
        if (service == QByteArray(Q_MEDIASERVICE_CAMERA))
            res << "camera1" << "camera2";        
        return res;
    }

    QString deviceDescription(const QByteArray &service, const QByteArray &device)
    {
        if (devices(service).contains(device))
            return QString(device)+" description";
        else
            return QString();
    }
};

class MockServicePlugin2 : public QMediaServiceProviderPlugin,
                            public QMediaServiceSupportedFormatsInterface,
                            public QMediaServiceFeaturesInterface
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QMediaServiceSupportedFormatsInterface)
    Q_INTERFACES(QtMobility::QMediaServiceFeaturesInterface)
public:
    QStringList keys() const
    {
        return QStringList() << QLatin1String(Q_MEDIASERVICE_MEDIAPLAYER);
    }

    QMediaService* create(QString const& key)
    {
        if (keys().contains(key))
            return new MockMediaService("MockServicePlugin2");
        else
            return 0;
    }

    void release(QMediaService *service)
    {
        delete service;
    }

    QtMedia::SupportEstimate hasSupport(const QString &mimeType, const QStringList& codecs) const
    {
        Q_UNUSED(codecs);

        if (mimeType == "audio/wav")
            return QtMedia::PreferedService;        

        return QtMedia::NotSupported;
    }

    QMediaServiceProviderHint::Features supportedFeatures(const QByteArray &service) const
    {
        if (service == QByteArray(Q_MEDIASERVICE_MEDIAPLAYER))
            return QMediaServiceProviderHint::LowLatencyPlayback;
        else
            return 0;
    }
};


class MockServicePlugin3 : public QMediaServiceProviderPlugin,
                            public QMediaServiceSupportedDevicesInterface
{
    Q_OBJECT
    Q_INTERFACES(QtMobility::QMediaServiceSupportedDevicesInterface)
public:
    QStringList keys() const
    {
        return QStringList() <<
               QLatin1String(Q_MEDIASERVICE_MEDIAPLAYER) <<
               QLatin1String(Q_MEDIASERVICE_CAMERA) <<
               QLatin1String(Q_MEDIASERVICE_AUDIOSOURCE);
    }

    QMediaService* create(QString const& key)
    {
        if (keys().contains(key))
            return new MockMediaService("MockServicePlugin3");
        else
            return 0;
    }

    void release(QMediaService *service)
    {
        delete service;
    }

    QList<QByteArray> devices(const QByteArray &service) const
    {
        QList<QByteArray> res;
        if (service == QByteArray(Q_MEDIASERVICE_CAMERA))
            res << "camera3" << "camera4";
        else if (service == QByteArray(Q_MEDIASERVICE_AUDIOSOURCE))
            res << "audiosource1" << "audiosource2";

        return res;
    }

    QString deviceDescription(const QByteArray &service, const QByteArray &device)
    {
        if (devices(service).contains(device))
            return QString(device)+" description";
        else
            return QString();
    }
};



class MockMediaServiceProvider : public QMediaServiceProvider
{
    QMediaService* requestService(const QByteArray &type, const QMediaServiceProviderHint &)
    {
        Q_UNUSED(type);
        return 0;
    }

    void releaseService(QMediaService *service)
    {
        Q_UNUSED(service);
    }
};


class tst_QMediaServiceProvider : public QObject
{
    Q_OBJECT

public slots:
    void initTestCase();

private slots:    
    void testDefaultProviderAvailable();
    void testObtainService();
    void testHasSupport();
    void testDevices();
    void testProviderHints();

private:
    QObjectList plugins;
};

void tst_QMediaServiceProvider::initTestCase()
{
    plugins << new MockServicePlugin1;
    plugins << new MockServicePlugin2;
    plugins << new MockServicePlugin3;

    QMediaPluginLoader::setStaticPlugins(QLatin1String("/mediaservice"), plugins);
}

void tst_QMediaServiceProvider::testDefaultProviderAvailable()
{
    // Must always be a default provider available    
    QVERIFY(QMediaServiceProvider::defaultServiceProvider() != 0);
}

void tst_QMediaServiceProvider::testObtainService()
{
    QMediaServiceProvider *provider = QMediaServiceProvider::defaultServiceProvider();

    if (provider == 0)
        QSKIP("No default provider", SkipSingle);

    QMediaService *service = 0;

    QTest::ignoreMessage(QtWarningMsg, "Load static plugins for \"/mediaservice/\" ");
    // Player
    service = provider->requestService(Q_MEDIASERVICE_MEDIAPLAYER);
    QVERIFY(service != 0);
    provider->releaseService(service);
}

void tst_QMediaServiceProvider::testHasSupport()
{
    MockMediaServiceProvider mockProvider;
    QCOMPARE(mockProvider.hasSupport(QByteArray(Q_MEDIASERVICE_MEDIAPLAYER), "video/ogv", QStringList()),
             QtMedia::MaybeSupported);

    QMediaServiceProvider *provider = QMediaServiceProvider::defaultServiceProvider();

    if (provider == 0)
        QSKIP("No default provider", SkipSingle);

    QCOMPARE(provider->hasSupport(QByteArray(Q_MEDIASERVICE_MEDIAPLAYER), "video/ogv", QStringList()),
             QtMedia::MaybeSupported);

    QCOMPARE(provider->hasSupport(QByteArray(Q_MEDIASERVICE_MEDIAPLAYER), "audio/ogg", QStringList()),
             QtMedia::ProbablySupported);

    //while the service returns PreferredService, provider should return ProbablySupported
    QCOMPARE(provider->hasSupport(QByteArray(Q_MEDIASERVICE_MEDIAPLAYER), "audio/wav", QStringList()),
             QtMedia::ProbablySupported);

    //even while all the plugins with "hasSupport" returned NotSupported,
    //MockServicePlugin3 has no "hasSupport" interface, so MaybeSupported
    QCOMPARE(provider->hasSupport(QByteArray(Q_MEDIASERVICE_MEDIAPLAYER), "video/avi",
                                  QStringList() << "mpeg4"),
             QtMedia::MaybeSupported);

    QCOMPARE(provider->hasSupport(QByteArray("non existing service"), "video/ogv", QStringList()),
             QtMedia::NotSupported);

    QCOMPARE(QMediaPlayer::hasSupport("video/ogv"), QtMedia::MaybeSupported);    
    QCOMPARE(QMediaPlayer::hasSupport("audio/ogg"), QtMedia::ProbablySupported);
    QCOMPARE(QMediaPlayer::hasSupport("audio/wav"), QtMedia::ProbablySupported);

    //ensure the correct media player plugin is choosen for mime type
    QMediaPlayer simplePlayer(0, QMediaPlayer::LowLatency);
    QCOMPARE(simplePlayer.service()->objectName(), QLatin1String("MockServicePlugin2"));

    QMediaPlayer mediaPlayer;
    QVERIFY(mediaPlayer.service()->objectName() != QLatin1String("MockServicePlugin2"));
}

void tst_QMediaServiceProvider::testDevices()
{
    MockMediaServiceProvider mockProvider;
    QVERIFY(mockProvider.devices(QByteArray(Q_MEDIASERVICE_CAMERA)).isEmpty());
    QVERIFY(mockProvider.deviceDescription(QByteArray(Q_MEDIASERVICE_CAMERA),
                                           QByteArray()).isEmpty());

    QMediaServiceProvider *provider = QMediaServiceProvider::defaultServiceProvider();

    QList<QByteArray> cameraDevices = provider->devices(QByteArray(Q_MEDIASERVICE_CAMERA));
    QCOMPARE(cameraDevices.count(), 4);
    QVERIFY(cameraDevices.contains(QByteArray("camera1")));
    QVERIFY(cameraDevices.contains(QByteArray("camera2")));
    QVERIFY(cameraDevices.contains(QByteArray("camera3")));
    QVERIFY(cameraDevices.contains(QByteArray("camera4")));

    //ensure the right plugin is choosen for a device
    QCamera camera1(QByteArray("camera1"));
    QCOMPARE( camera1.service()->objectName(), QLatin1String("MockServicePlugin1") );
    QCamera camera2(QByteArray("camera2"));
    QCOMPARE( camera2.service()->objectName(), QLatin1String("MockServicePlugin1") );
    QCamera camera3(QByteArray("camera3"));
    QCOMPARE( camera3.service()->objectName(), QLatin1String("MockServicePlugin3") );
    QCamera camera4(QByteArray("camera4"));
    QCOMPARE( camera4.service()->objectName(), QLatin1String("MockServicePlugin3") );

    QList<QByteArray> audioSourceDevices = provider->devices(QByteArray(Q_MEDIASERVICE_AUDIOSOURCE));
    QCOMPARE(audioSourceDevices.count(), 2);
    QVERIFY(audioSourceDevices.contains(QByteArray("audiosource1")));
    QVERIFY(audioSourceDevices.contains(QByteArray("audiosource2")));

    QVERIFY(provider->devices(QByteArray("non existing service")).isEmpty());
}




void tst_QMediaServiceProvider::testProviderHints()
{
    {
        QMediaServiceProviderHint hint;
        QVERIFY(hint.isNull());
        QCOMPARE(hint.type(), QMediaServiceProviderHint::Null);
        QVERIFY(hint.device().isEmpty());
        QVERIFY(hint.mimeType().isEmpty());
        QVERIFY(hint.codecs().isEmpty());
        QCOMPARE(hint.features(), 0);
    }

    {
        QByteArray deviceName(QByteArray("testDevice"));
        QMediaServiceProviderHint hint(deviceName);
        QVERIFY(!hint.isNull());
        QCOMPARE(hint.type(), QMediaServiceProviderHint::Device);
        QCOMPARE(hint.device(), deviceName);
        QVERIFY(hint.mimeType().isEmpty());
        QVERIFY(hint.codecs().isEmpty());
        QCOMPARE(hint.features(), 0);
    }

    {
        QMediaServiceProviderHint hint(QMediaServiceProviderHint::LowLatencyPlayback);
        QVERIFY(!hint.isNull());
        QCOMPARE(hint.type(), QMediaServiceProviderHint::SupportedFeatures);
        QVERIFY(hint.device().isEmpty());
        QVERIFY(hint.mimeType().isEmpty());
        QVERIFY(hint.codecs().isEmpty());
        QCOMPARE(hint.features(), QMediaServiceProviderHint::LowLatencyPlayback);
    }

    {
        QString mimeType(QLatin1String("video/ogg"));
        QStringList codecs;
        codecs << "theora" << "vorbis";

        QMediaServiceProviderHint hint(mimeType,codecs);
        QVERIFY(!hint.isNull());
        QCOMPARE(hint.type(), QMediaServiceProviderHint::ContentType);
        QVERIFY(hint.device().isEmpty());
        QCOMPARE(hint.mimeType(), mimeType);
        QCOMPARE(hint.codecs(), codecs);

        QMediaServiceProviderHint hint2(hint);

        QVERIFY(!hint2.isNull());
        QCOMPARE(hint2.type(), QMediaServiceProviderHint::ContentType);
        QVERIFY(hint2.device().isEmpty());
        QCOMPARE(hint2.mimeType(), mimeType);
        QCOMPARE(hint2.codecs(), codecs);

        QMediaServiceProviderHint hint3;
        QVERIFY(hint3.isNull());
        hint3 = hint;
        QVERIFY(!hint3.isNull());
        QCOMPARE(hint3.type(), QMediaServiceProviderHint::ContentType);
        QVERIFY(hint3.device().isEmpty());
        QCOMPARE(hint3.mimeType(), mimeType);
        QCOMPARE(hint3.codecs(), codecs);

        QCOMPARE(hint, hint2);
        QCOMPARE(hint3, hint2);

        QMediaServiceProviderHint hint4(mimeType,codecs);
        QCOMPARE(hint, hint4);

        QMediaServiceProviderHint hint5(mimeType,QStringList());
        QVERIFY(hint != hint5);
    }
}

QTEST_MAIN(tst_QMediaServiceProvider)

#include "tst_qmediaserviceprovider.moc"