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

//TESTED_COMPONENT=src/multimedia

#include "tst_qmediaplayer.h"

#include <qgraphicsvideoitem.h>
#include <QtNetwork/qnetworkconfigmanager.h>

// Encouraging successful diversity through copy and paste.
#ifndef QTRY_COMPARE
#define QTRY_COMPARE(__expr, __expected) \
    do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if ((__expr) != (__expected)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && ((__expr) != (__expected)); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QCOMPARE(__expr, __expected); \
    } while(0)
#endif

#ifndef QTRY_VERIFY
#define QTRY_VERIFY(__expr) \
    do { \
        const int __step = 50; \
        const int __timeout = 5000; \
        if (!(__expr)) { \
            QTest::qWait(0); \
        } \
        for (int __i = 0; __i < __timeout && !(__expr); __i+=__step) { \
            QTest::qWait(__step); \
        } \
        QVERIFY(__expr); \
    } while(0)
#endif

QT_USE_NAMESPACE

#include "mockvideosurface.h"

void tst_QMediaPlayer::initTestCase_data()
{
    QTest::addColumn<bool>("valid");
    QTest::addColumn<QMediaPlayer::State>("state");
    QTest::addColumn<QMediaPlayer::MediaStatus>("status");
    QTest::addColumn<QMediaContent>("mediaContent");
    QTest::addColumn<qint64>("duration");
    QTest::addColumn<qint64>("position");
    QTest::addColumn<bool>("seekable");
    QTest::addColumn<int>("volume");
    QTest::addColumn<bool>("muted");
    QTest::addColumn<bool>("videoAvailable");
    QTest::addColumn<int>("bufferStatus");
    QTest::addColumn<qreal>("playbackRate");
    QTest::addColumn<QMediaPlayer::Error>("error");
    QTest::addColumn<QString>("errorString");

    QTest::newRow("invalid") << false << QMediaPlayer::StoppedState << QMediaPlayer::UnknownMediaStatus <<
                                QMediaContent() << qint64(0) << qint64(0) << false << 0 << false << false << 0 <<
                                qreal(0) << QMediaPlayer::NoError << QString();
    QTest::newRow("valid+null") << true << QMediaPlayer::StoppedState << QMediaPlayer::UnknownMediaStatus <<
                                QMediaContent() << qint64(0) << qint64(0) << false << 0 << false << false << 50 <<
                                qreal(0) << QMediaPlayer::NoError << QString();
    QTest::newRow("valid+content+stopped") << true << QMediaPlayer::StoppedState << QMediaPlayer::UnknownMediaStatus <<
                                QMediaContent(QUrl("file:///some.mp3")) << qint64(0) << qint64(0) << false << 50 << false << false << 0 <<
                                qreal(1) << QMediaPlayer::NoError << QString();
    QTest::newRow("valid+content+playing") << true << QMediaPlayer::PlayingState << QMediaPlayer::LoadedMedia <<
                                QMediaContent(QUrl("file:///some.mp3")) << qint64(10000) << qint64(10) << true << 50 << true << false << 0 <<
                                qreal(1) << QMediaPlayer::NoError << QString();
    QTest::newRow("valid+content+paused") << true << QMediaPlayer::PausedState << QMediaPlayer::LoadedMedia <<
                                QMediaContent(QUrl("file:///some.mp3")) << qint64(10000) << qint64(10) << true << 50 << true << false << 0 <<
                                qreal(1)  << QMediaPlayer::NoError << QString();
    QTest::newRow("valud+streaming") << true << QMediaPlayer::PlayingState << QMediaPlayer::LoadedMedia <<
                                QMediaContent(QUrl("http://example.com/stream")) << qint64(10000) << qint64(10000) << false << 50 << false << true << 0 <<
                                qreal(1)  << QMediaPlayer::NoError << QString();
    QTest::newRow("valid+error") << true << QMediaPlayer::StoppedState << QMediaPlayer::UnknownMediaStatus <<
                                QMediaContent(QUrl("http://example.com/stream")) << qint64(0) << qint64(0) << false << 50 << false << false << 0 <<
                                qreal(0) << QMediaPlayer::ResourceError << QString("Resource unavailable");
}

void tst_QMediaPlayer::initTestCase()
{
    qRegisterMetaType<QMediaPlayer::State>("QMediaPlayer::State");
    qRegisterMetaType<QMediaPlayer::Error>("QMediaPlayer::Error");
    qRegisterMetaType<QMediaPlayer::MediaStatus>("QMediaPlayer::MediaStatus");
    qRegisterMetaType<QMediaContent>("QMediaContent");

    mockService = new MockMediaPlayerService;
    mockProvider = new MockMediaServiceProvider(mockService, true);
    player = new QMediaPlayer(0, 0, mockProvider);
}

void tst_QMediaPlayer::cleanupTestCase()
{
    delete player;
}

void tst_QMediaPlayer::init()
{
    mockService->reset();
}

void tst_QMediaPlayer::cleanup()
{
}

void tst_QMediaPlayer::testNullService()
{
    MockMediaServiceProvider provider(0);
    QMediaPlayer player(0, 0, &provider);

    const QIODevice *nullDevice = 0;

    QCOMPARE(player.media(), QMediaContent());
    QCOMPARE(player.mediaStream(), nullDevice);
    QCOMPARE(player.state(), QMediaPlayer::StoppedState);
    QCOMPARE(player.mediaStatus(), QMediaPlayer::UnknownMediaStatus);
    QCOMPARE(player.duration(), qint64(-1));
    QCOMPARE(player.position(), qint64(0));
    QCOMPARE(player.volume(), 0);
    QCOMPARE(player.isMuted(), false);
    QCOMPARE(player.isVideoAvailable(), false);
    QCOMPARE(player.bufferStatus(), 0);
    QCOMPARE(player.isSeekable(), false);
    QCOMPARE(player.playbackRate(), qreal(0));
    QCOMPARE(player.error(), QMediaPlayer::ServiceMissingError);
    QCOMPARE(player.isAvailable(), false);
    QCOMPARE(player.availabilityError(), QtMultimediaKit::ServiceMissingError);

    {
        QFETCH_GLOBAL(QMediaContent, mediaContent);

        QSignalSpy spy(&player, SIGNAL(mediaChanged(QMediaContent)));
        QFile file;

        player.setMedia(mediaContent, &file);
        QCOMPARE(player.media(), QMediaContent());
        QCOMPARE(player.mediaStream(), nullDevice);
        QCOMPARE(spy.count(), 0);
    } {
        QSignalSpy stateSpy(&player, SIGNAL(stateChanged(QMediaPlayer::State)));
        QSignalSpy statusSpy(&player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)));

        player.play();
        QCOMPARE(player.state(), QMediaPlayer::StoppedState);
        QCOMPARE(player.mediaStatus(), QMediaPlayer::UnknownMediaStatus);
        QCOMPARE(stateSpy.count(), 0);
        QCOMPARE(statusSpy.count(), 0);

        player.pause();
        QCOMPARE(player.state(), QMediaPlayer::StoppedState);
        QCOMPARE(player.mediaStatus(), QMediaPlayer::UnknownMediaStatus);
        QCOMPARE(stateSpy.count(), 0);
        QCOMPARE(statusSpy.count(), 0);

        player.stop();
        QCOMPARE(player.state(), QMediaPlayer::StoppedState);
        QCOMPARE(player.mediaStatus(), QMediaPlayer::UnknownMediaStatus);
        QCOMPARE(stateSpy.count(), 0);
        QCOMPARE(statusSpy.count(), 0);
    } {
        QFETCH_GLOBAL(int, volume);
        QFETCH_GLOBAL(bool, muted);

        QSignalSpy volumeSpy(&player, SIGNAL(volumeChanged(int)));
        QSignalSpy mutingSpy(&player, SIGNAL(mutedChanged(bool)));

        player.setVolume(volume);
        QCOMPARE(player.volume(), 0);
        QCOMPARE(volumeSpy.count(), 0);

        player.setMuted(muted);
        QCOMPARE(player.isMuted(), false);
        QCOMPARE(mutingSpy.count(), 0);
    } {
        QFETCH_GLOBAL(qint64, position);

        QSignalSpy spy(&player, SIGNAL(positionChanged(qint64)));

        player.setPosition(position);
        QCOMPARE(player.position(), qint64(0));
        QCOMPARE(spy.count(), 0);
    } {
        QFETCH_GLOBAL(qreal, playbackRate);

        QSignalSpy spy(&player, SIGNAL(playbackRateChanged(qreal)));

        player.setPlaybackRate(playbackRate);
        QCOMPARE(player.playbackRate(), qreal(0));
        QCOMPARE(spy.count(), 0);
    } {
        QMediaPlaylist playlist;
        player.setPlaylist(&playlist);

        QSignalSpy mediaSpy(&player, SIGNAL(mediaChanged(QMediaContent)));
        QSignalSpy statusSpy(&player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)));

        playlist.addMedia(QUrl("http://example.com/stream"));
        playlist.addMedia(QUrl("file:///some.mp3"));

        playlist.setCurrentIndex(0);
        QCOMPARE(playlist.currentIndex(), 0);
        QCOMPARE(player.media(), QMediaContent());
        QCOMPARE(mediaSpy.count(), 0);
        QCOMPARE(statusSpy.count(), 0);

        playlist.next();
        QCOMPARE(playlist.currentIndex(), 1);
        QCOMPARE(player.media(), QMediaContent());
        QCOMPARE(mediaSpy.count(), 0);
        QCOMPARE(statusSpy.count(), 0);
    }
}

void tst_QMediaPlayer::testValid()
{
    /*
    QFETCH_GLOBAL(bool, valid);

    mockService->setIsValid(valid);
    QCOMPARE(player->isValid(), valid);
    */
}

void tst_QMediaPlayer::testMedia()
{
    QFETCH_GLOBAL(QMediaContent, mediaContent);

    mockService->setMedia(mediaContent);
    QCOMPARE(player->media(), mediaContent);

    QBuffer stream;
    player->setMedia(mediaContent, &stream);
    QCOMPARE(player->media(), mediaContent);
    QCOMPARE((QBuffer*)player->mediaStream(), &stream);
}

void tst_QMediaPlayer::testDuration()
{
    QFETCH_GLOBAL(qint64, duration);

    mockService->setDuration(duration);
    QVERIFY(player->duration() == duration);
}

void tst_QMediaPlayer::testPosition()
{
    QFETCH_GLOBAL(bool, valid);
    QFETCH_GLOBAL(bool, seekable);
    QFETCH_GLOBAL(qint64, position);
    QFETCH_GLOBAL(qint64, duration);

    mockService->setIsValid(valid);
    mockService->setSeekable(seekable);
    mockService->setPosition(position);
    mockService->setDuration(duration);
    QVERIFY(player->isSeekable() == seekable);
    QVERIFY(player->position() == position);
    QVERIFY(player->duration() == duration);

    if (seekable) {
        { QSignalSpy spy(player, SIGNAL(positionChanged(qint64)));
        player->setPosition(position);
        QCOMPARE(player->position(), position);
        QCOMPARE(spy.count(), 0); }

        mockService->setPosition(position);
        { QSignalSpy spy(player, SIGNAL(positionChanged(qint64)));
        player->setPosition(0);
        QCOMPARE(player->position(), qint64(0));
        QCOMPARE(spy.count(), position == 0 ? 0 : 1); }

        mockService->setPosition(position);
        { QSignalSpy spy(player, SIGNAL(positionChanged(qint64)));
        player->setPosition(duration);
        QCOMPARE(player->position(), duration);
        QCOMPARE(spy.count(), position == duration ? 0 : 1); }

        mockService->setPosition(position);
        { QSignalSpy spy(player, SIGNAL(positionChanged(qint64)));
        player->setPosition(-1);
        QCOMPARE(player->position(), qint64(0));
        QCOMPARE(spy.count(), position == 0 ? 0 : 1); }

        mockService->setPosition(position);
        { QSignalSpy spy(player, SIGNAL(positionChanged(qint64)));
        player->setPosition(duration + 1);
        QCOMPARE(player->position(), duration);
        QCOMPARE(spy.count(), position == duration ? 0 : 1); }
    }
    else {
        QSignalSpy spy(player, SIGNAL(positionChanged(qint64)));
        player->setPosition(position);

        QCOMPARE(player->position(), position);
        QCOMPARE(spy.count(), 0);
    }
}

void tst_QMediaPlayer::testVolume()
{
    QFETCH_GLOBAL(bool, valid);
    QFETCH_GLOBAL(int, volume);

    mockService->setVolume(volume);
    QVERIFY(player->volume() == volume);

    if (valid) {
        { QSignalSpy spy(player, SIGNAL(volumeChanged(int)));
        player->setVolume(10);
        QCOMPARE(player->volume(), 10);
        QCOMPARE(spy.count(), 1); }

        { QSignalSpy spy(player, SIGNAL(volumeChanged(int)));
        player->setVolume(-1000);
        QCOMPARE(player->volume(), 0);
        QCOMPARE(spy.count(), 1); }

        { QSignalSpy spy(player, SIGNAL(volumeChanged(int)));
        player->setVolume(100);
        QCOMPARE(player->volume(), 100);
        QCOMPARE(spy.count(), 1); }

        { QSignalSpy spy(player, SIGNAL(volumeChanged(int)));
        player->setVolume(1000);
        QCOMPARE(player->volume(), 100);
        QCOMPARE(spy.count(), 0); }
    }
}

void tst_QMediaPlayer::testMuted()
{
    QFETCH_GLOBAL(bool, valid);
    QFETCH_GLOBAL(bool, muted);
    QFETCH_GLOBAL(int, volume);

    if (valid) {
        mockService->setMuted(muted);
        mockService->setVolume(volume);
        QVERIFY(player->isMuted() == muted);

        QSignalSpy spy(player, SIGNAL(mutedChanged(bool)));
        player->setMuted(!muted);
        QCOMPARE(player->isMuted(), !muted);
        QCOMPARE(player->volume(), volume);
        QCOMPARE(spy.count(), 1);
    }
}

void tst_QMediaPlayer::testVideoAvailable()
{
    QFETCH_GLOBAL(bool, videoAvailable);

    mockService->setVideoAvailable(videoAvailable);
    QVERIFY(player->isVideoAvailable() == videoAvailable);
}

void tst_QMediaPlayer::testBufferStatus()
{
    QFETCH_GLOBAL(int, bufferStatus);

    mockService->setBufferStatus(bufferStatus);
    QVERIFY(player->bufferStatus() == bufferStatus);
}

void tst_QMediaPlayer::testSeekable()
{
    QFETCH_GLOBAL(bool, seekable);

    mockService->setSeekable(seekable);
    QVERIFY(player->isSeekable() == seekable);
}

void tst_QMediaPlayer::testPlaybackRate()
{
    QFETCH_GLOBAL(bool, valid);
    QFETCH_GLOBAL(qreal, playbackRate);

    if (valid) {
        mockService->setPlaybackRate(playbackRate);
        QVERIFY(player->playbackRate() == playbackRate);

        QSignalSpy spy(player, SIGNAL(playbackRateChanged(qreal)));
        player->setPlaybackRate(playbackRate + 0.5f);
        QCOMPARE(player->playbackRate(), playbackRate + 0.5f);
        QCOMPARE(spy.count(), 1);
    }
}

void tst_QMediaPlayer::testError()
{
    QFETCH_GLOBAL(QMediaPlayer::Error, error);

    mockService->setError(error);
    QVERIFY(player->error() == error);
}

void tst_QMediaPlayer::testErrorString()
{
    QFETCH_GLOBAL(QString, errorString);

    mockService->setErrorString(errorString);
    QVERIFY(player->errorString() == errorString);
}

void tst_QMediaPlayer::testIsAvailable()
{
    QCOMPARE(player->isAvailable(), true);
    QCOMPARE(player->availabilityError(), QtMultimediaKit::NoError);
}

void tst_QMediaPlayer::testService()
{
    /*
    QFETCH_GLOBAL(bool, valid);

    mockService->setIsValid(valid);

    if (valid)
        QVERIFY(player->service() != 0);
    else
        QVERIFY(player->service() == 0);
        */
}

void tst_QMediaPlayer::testPlay()
{
    QFETCH_GLOBAL(bool, valid);
    QFETCH_GLOBAL(QMediaContent, mediaContent);
    QFETCH_GLOBAL(QMediaPlayer::State, state);

    mockService->setIsValid(valid);
    mockService->setState(state);
    mockService->setMedia(mediaContent);
    QVERIFY(player->state() == state);
    QVERIFY(player->media() == mediaContent);

    QSignalSpy spy(player, SIGNAL(stateChanged(QMediaPlayer::State)));

    player->play();

    if (!valid || mediaContent.isNull())  {
        QCOMPARE(player->state(), QMediaPlayer::StoppedState);
        QCOMPARE(spy.count(), 0);
    }
    else {
        QCOMPARE(player->state(), QMediaPlayer::PlayingState);
        QCOMPARE(spy.count(), state == QMediaPlayer::PlayingState ? 0 : 1);
    }
}

void tst_QMediaPlayer::testPause()
{
    QFETCH_GLOBAL(bool, valid);
    QFETCH_GLOBAL(QMediaContent, mediaContent);
    QFETCH_GLOBAL(QMediaPlayer::State, state);

    mockService->setIsValid(valid);
    mockService->setState(state);
    mockService->setMedia(mediaContent);
    QVERIFY(player->state() == state);
    QVERIFY(player->media() == mediaContent);

    QSignalSpy spy(player, SIGNAL(stateChanged(QMediaPlayer::State)));

    player->pause();

    if (!valid || mediaContent.isNull()) {
        QCOMPARE(player->state(), QMediaPlayer::StoppedState);
        QCOMPARE(spy.count(), 0);
    }
    else {
        QCOMPARE(player->state(), QMediaPlayer::PausedState);
        QCOMPARE(spy.count(), state == QMediaPlayer::PausedState ? 0 : 1);
    }
}

void tst_QMediaPlayer::testStop()
{
    QFETCH_GLOBAL(QMediaContent, mediaContent);
    QFETCH_GLOBAL(QMediaPlayer::State, state);

    mockService->setState(state);
    mockService->setMedia(mediaContent);
    QVERIFY(player->state() == state);
    QVERIFY(player->media() == mediaContent);

    QSignalSpy spy(player, SIGNAL(stateChanged(QMediaPlayer::State)));

    player->stop();

    if (mediaContent.isNull() || state == QMediaPlayer::StoppedState) {
        QCOMPARE(player->state(), QMediaPlayer::StoppedState);
        QCOMPARE(spy.count(), 0);
    }
    else {
        QCOMPARE(player->state(), QMediaPlayer::StoppedState);
        QCOMPARE(spy.count(), 1);
    }
}

void tst_QMediaPlayer::testMediaStatus()
{
    QFETCH_GLOBAL(int, bufferStatus);
    int bufferSignals = 0;

    player->setNotifyInterval(10);

    mockService->setMediaStatus(QMediaPlayer::NoMedia);
    mockService->setBufferStatus(bufferStatus);

    AutoConnection connection(
            player, SIGNAL(bufferStatusChanged(int)),
            &QTestEventLoop::instance(), SLOT(exitLoop()));

    QSignalSpy statusSpy(player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)));
    QSignalSpy bufferSpy(player, SIGNAL(bufferStatusChanged(int)));

    QCOMPARE(player->mediaStatus(), QMediaPlayer::NoMedia);

    mockService->setMediaStatus(QMediaPlayer::LoadingMedia);
    QCOMPARE(player->mediaStatus(), QMediaPlayer::LoadingMedia);
    QCOMPARE(statusSpy.count(), 1);

    QCOMPARE(qvariant_cast<QMediaPlayer::MediaStatus>(statusSpy.last().value(0)),
             QMediaPlayer::LoadingMedia);

    mockService->setMediaStatus(QMediaPlayer::LoadedMedia);
    QCOMPARE(player->mediaStatus(), QMediaPlayer::LoadedMedia);
    QCOMPARE(statusSpy.count(), 2);

    QCOMPARE(qvariant_cast<QMediaPlayer::MediaStatus>(statusSpy.last().value(0)),
             QMediaPlayer::LoadedMedia);

    // Verify the bufferStatusChanged() signal isn't being emitted.
    QTestEventLoop::instance().enterLoop(1);
    QCOMPARE(bufferSpy.count(), 0);

    mockService->setMediaStatus(QMediaPlayer::StalledMedia);
    QCOMPARE(player->mediaStatus(), QMediaPlayer::StalledMedia);
    QCOMPARE(statusSpy.count(), 3);

    QCOMPARE(qvariant_cast<QMediaPlayer::MediaStatus>(statusSpy.last().value(0)),
             QMediaPlayer::StalledMedia);

    // Verify the bufferStatusChanged() signal is being emitted.
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(bufferSpy.count() > bufferSignals);
    QCOMPARE(bufferSpy.last().value(0).toInt(), bufferStatus);
    bufferSignals = bufferSpy.count();

    mockService->setMediaStatus(QMediaPlayer::BufferingMedia);
    QCOMPARE(player->mediaStatus(), QMediaPlayer::BufferingMedia);
    QCOMPARE(statusSpy.count(), 4);

    QCOMPARE(qvariant_cast<QMediaPlayer::MediaStatus>(statusSpy.last().value(0)),
             QMediaPlayer::BufferingMedia);

    // Verify the bufferStatusChanged() signal is being emitted.
    QTestEventLoop::instance().enterLoop(1);
    QVERIFY(bufferSpy.count() > bufferSignals);
    QCOMPARE(bufferSpy.last().value(0).toInt(), bufferStatus);
    bufferSignals = bufferSpy.count();

    mockService->setMediaStatus(QMediaPlayer::BufferedMedia);
    QCOMPARE(player->mediaStatus(), QMediaPlayer::BufferedMedia);
    QCOMPARE(statusSpy.count(), 5);

    QCOMPARE(qvariant_cast<QMediaPlayer::MediaStatus>(statusSpy.last().value(0)),
             QMediaPlayer::BufferedMedia);

    // Verify the bufferStatusChanged() signal isn't being emitted.
    QTestEventLoop::instance().enterLoop(1);
    QCOMPARE(bufferSpy.count(), bufferSignals);

    mockService->setMediaStatus(QMediaPlayer::EndOfMedia);
    QCOMPARE(player->mediaStatus(), QMediaPlayer::EndOfMedia);
    QCOMPARE(statusSpy.count(), 6);

    QCOMPARE(qvariant_cast<QMediaPlayer::MediaStatus>(statusSpy.last().value(0)),
             QMediaPlayer::EndOfMedia);
}

void tst_QMediaPlayer::testPlaylist()
{
    QMediaContent content0(QUrl(QLatin1String("test://audio/song1.mp3")));
    QMediaContent content1(QUrl(QLatin1String("test://audio/song2.mp3")));
    QMediaContent content2(QUrl(QLatin1String("test://video/movie1.mp4")));
    QMediaContent content3(QUrl(QLatin1String("test://video/movie2.mp4")));
    QMediaContent content4(QUrl(QLatin1String("test://image/photo.jpg")));

    mockService->setIsValid(true);
    mockService->setState(QMediaPlayer::StoppedState, QMediaPlayer::NoMedia);

    QMediaPlaylist *playlist = new QMediaPlaylist;
    player->setPlaylist(playlist);

    QSignalSpy stateSpy(player, SIGNAL(stateChanged(QMediaPlayer::State)));
    QSignalSpy mediaSpy(player, SIGNAL(mediaChanged(QMediaContent)));

    // Test the player does nothing with an empty playlist attached.
    player->play();
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);
    QCOMPARE(player->media(), QMediaContent());
    QCOMPARE(stateSpy.count(), 0);
    QCOMPARE(mediaSpy.count(), 0);

    playlist->addMedia(content0);
    playlist->addMedia(content1);
    playlist->addMedia(content2);
    playlist->addMedia(content3);

    // Test changing the playlist position, changes the current media, but not the playing state.
    playlist->setCurrentIndex(1);
    QCOMPARE(player->media(), content1);
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 0);
    QCOMPARE(mediaSpy.count(), 1);

    // Test playing starts with the current media.
    player->play();
    QCOMPARE(player->media(), content1);
    QCOMPARE(player->state(), QMediaPlayer::PlayingState);
    QCOMPARE(stateSpy.count(), 1);
    QCOMPARE(mediaSpy.count(), 1);

    // Test pausing doesn't change the current media.
    player->pause();
    QCOMPARE(player->media(), content1);
    QCOMPARE(player->state(), QMediaPlayer::PausedState);
    QCOMPARE(stateSpy.count(), 2);
    QCOMPARE(mediaSpy.count(), 1);

    // Test stopping doesn't change the current media.
    player->stop();
    QCOMPARE(player->media(), content1);
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 3);
    QCOMPARE(mediaSpy.count(), 1);

    // Test when the player service reaches the end of the current media, the player moves onto
    // the next item without stopping.
    player->play();
    QCOMPARE(player->media(), content1);
    QCOMPARE(player->state(), QMediaPlayer::PlayingState);
    QCOMPARE(stateSpy.count(), 4);
    QCOMPARE(mediaSpy.count(), 1);

    mockService->setState(QMediaPlayer::StoppedState, QMediaPlayer::EndOfMedia);
    QCOMPARE(player->media(), content2);
    QCOMPARE(player->state(), QMediaPlayer::PlayingState);
    QCOMPARE(stateSpy.count(), 4);
    QCOMPARE(mediaSpy.count(), 2);

    // Test skipping the current media doesn't change the state.
    playlist->next();
    QCOMPARE(player->media(), content3);
    QCOMPARE(player->state(), QMediaPlayer::PlayingState);
    QCOMPARE(stateSpy.count(), 4);
    QCOMPARE(mediaSpy.count(), 3);

    // Test changing the current media while paused doesn't change the state.
    player->pause();
    mockService->setMediaStatus(QMediaPlayer::BufferedMedia);
    QCOMPARE(player->media(), content3);
    QCOMPARE(player->state(), QMediaPlayer::PausedState);
    QCOMPARE(stateSpy.count(), 5);
    QCOMPARE(mediaSpy.count(), 3);

    playlist->previous();
    QCOMPARE(player->media(), content2);
    QCOMPARE(player->state(), QMediaPlayer::PausedState);
    QCOMPARE(stateSpy.count(), 5);
    QCOMPARE(mediaSpy.count(), 4);

    // Test changing the current media while stopped doesn't change the state.
    player->stop();
    mockService->setMediaStatus(QMediaPlayer::LoadedMedia);
    QCOMPARE(player->media(), content2);
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 6);
    QCOMPARE(mediaSpy.count(), 4);

    playlist->next();
    QCOMPARE(player->media(), content3);
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 6);
    QCOMPARE(mediaSpy.count(), 5);

    // Test the player is stopped and the current media cleared when it reaches the end of the last
    // item in the playlist.
    player->play();
    QCOMPARE(player->media(), content3);
    QCOMPARE(player->state(), QMediaPlayer::PlayingState);
    QCOMPARE(stateSpy.count(), 7);
    QCOMPARE(mediaSpy.count(), 5);

    // Double up the signals to ensure some noise doesn't destabalize things.
    mockService->setState(QMediaPlayer::StoppedState, QMediaPlayer::EndOfMedia);
    mockService->setState(QMediaPlayer::StoppedState, QMediaPlayer::EndOfMedia);
    QCOMPARE(player->media(), QMediaContent());
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 8);
    QCOMPARE(mediaSpy.count(), 6);

    // Test starts playing from the start of the playlist if there is no current media selected.
    player->play();
    QCOMPARE(player->media(), content0);
    QCOMPARE(player->state(), QMediaPlayer::PlayingState);
    QCOMPARE(stateSpy.count(), 9);
    QCOMPARE(mediaSpy.count(), 7);

    // Test deleting the playlist stops the player and clears the media it set.
    delete playlist;
    QCOMPARE(player->media(), QMediaContent());
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 10);
    QCOMPARE(mediaSpy.count(), 8);

    // Test the player works as normal with the playlist removed.
    player->play();
    QCOMPARE(player->media(), QMediaContent());
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);
    QCOMPARE(stateSpy.count(), 10);
    QCOMPARE(mediaSpy.count(), 8);

    player->setMedia(content1);
    player->play();

    QCOMPARE(player->media(), content1);
    QCOMPARE(player->state(), QMediaPlayer::PlayingState);
    QCOMPARE(stateSpy.count(), 11);
    QCOMPARE(mediaSpy.count(), 9);

    // Test the player can bind to playlist again
    playlist = new QMediaPlaylist;
    player->setPlaylist(playlist);

    QCOMPARE(player->media(), QMediaContent());
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);

    playlist->addMedia(content0);
    playlist->addMedia(content1);
    playlist->addMedia(content2);
    playlist->addMedia(content3);

    playlist->setCurrentIndex(1);
    QCOMPARE(player->media(), content1);
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);

    // Test attaching the new playlist,
    // player should detach the current one
    QMediaPlaylist *playlist2 = new QMediaPlaylist;
    playlist2->addMedia(content1);
    playlist2->addMedia(content2);
    playlist2->addMedia(content3);
    playlist2->setCurrentIndex(2);

    player->play();
    player->setPlaylist(playlist2);
    QCOMPARE(player->media(), playlist2->currentMedia());
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);

    playlist2->setCurrentIndex(1);
    QCOMPARE(player->media(), playlist2->currentMedia());

    {
        QMediaPlaylist playlist;
        playlist.addMedia(content1);
        playlist.addMedia(content2);
        playlist.addMedia(content3);
        playlist.setCurrentIndex(1);

        player->setPlaylist(&playlist);
        QCOMPARE(player->playlist(), &playlist);
        QCOMPARE(player->media(), content2);
    } //playlist should be detached now

    QVERIFY(player->playlist() == 0);
    QCOMPARE(player->media(), QMediaContent());

    // Test when the player service encounters an invalid media, the player moves onto
    // the next item without stopping
    {
        QSignalSpy ss(player, SIGNAL(stateChanged(QMediaPlayer::State)));
        QSignalSpy ms(player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)));

        player->setPlaylist(playlist);
        player->play();
        QCOMPARE(ss.count(), 1);

        mockService->setState(QMediaPlayer::StoppedState, QMediaPlayer::InvalidMedia);
        QCOMPARE(player->state(), QMediaPlayer::PlayingState);
        QCOMPARE(player->mediaStatus(), QMediaPlayer::InvalidMedia);
        QCOMPARE(ss.count(), 1);
        QCOMPARE(ms.count(), 1);

        // NOTE: status should begin transitioning through to BufferedMedia.
        QCOMPARE(player->media(), content2);
    }

}

void tst_QMediaPlayer::testPlayerFlags()
{
    MockMediaServiceProvider provider(0, true);
    QMediaPlayer::Flag flags = QMediaPlayer::LowLatency;

    QMediaPlayer player(0, flags, &provider);
    QMediaServiceProviderHint::Feature feature;

    if (flags & QMediaPlayer::LowLatency)
    {
        /* if the flag is low latency set the low latency play back for the service provider */
        feature = QMediaServiceProviderHint::LowLatencyPlayback;
        const QByteArray service(Q_MEDIASERVICE_MEDIAPLAYER);
        const QMediaServiceProviderHint providerHint(feature);
        /* request service for the service provider */
        provider.requestService(service,providerHint);

        /* Constructs a SupportedFeatures  media service provider hint. */
        QMediaServiceProviderHint servicepro(feature);

        /* compare the flag value */
        QVERIFY(servicepro.features() == QMediaServiceProviderHint::LowLatencyPlayback);
    }

    /* The player is expected to play QIODevice based streams.
        If passed to QMediaPlayer  constructor,
        the service supporting streams playback will be chosen. */
    flags = QMediaPlayer::StreamPlayback;
    /* Construct a QMediaPlayer  that uses the playback service from provider,
        parented to parent  and with flags.*/
    QMediaPlayer player2(0,flags , &provider);

    if (flags & QMediaPlayer::StreamPlayback)
    {
        /* if the flag is stream play back set the stream play back for the service provider */
        feature = QMediaServiceProviderHint::StreamPlayback;
        const QByteArray service(Q_MEDIASERVICE_MEDIAPLAYER);
        const QMediaServiceProviderHint providerHint(feature);

        /* request service for the service provider */
        provider.requestService(service,providerHint);

        /* Constructs a SupportedFeatures media service provider hint. */
        QMediaServiceProviderHint servicepro(feature);

        /* compare the flag value */
        QVERIFY(servicepro.features() == QMediaServiceProviderHint::StreamPlayback);
    }
}

void tst_QMediaPlayer::testDestructor()
{
    /* create an object for player */
    QMediaPlayer *player = new QMediaPlayer;

    /* check whether the object is created */
    QVERIFY(player);

    /* delete the instance */
    delete player;
}

void tst_QMediaPlayer::testNetworkAccess()
{
    QNetworkConfigurationManager manager;
    QList<QNetworkConfiguration> configs = manager.allConfigurations();

    if (configs.count() >= 1) {
        QSignalSpy spy(player, SIGNAL(networkConfigurationChanged(QNetworkConfiguration)));
        int index = qFloor((configs.count())/2);
        player->setNetworkConfigurations(configs);
        mockService->selectCurrentConfiguration(configs.at(index));

        QVERIFY(spy.count() == 1);
        QList<QVariant> args = spy.takeFirst();
        QNetworkConfiguration config = args.at(0).value<QNetworkConfiguration>();
        QCOMPARE(config.identifier() , configs.at(index).identifier());
        QCOMPARE(player->currentNetworkConfiguration().identifier() , config.identifier());
    }

    // invalidate current network configuration
    QSignalSpy spy(player, SIGNAL(networkConfigurationChanged(QNetworkConfiguration)));
    mockService->selectCurrentConfiguration(QNetworkConfiguration());
    QVERIFY(spy.count() == 1);
    QList<QVariant> args = spy.takeFirst();
    QNetworkConfiguration config = args.at(0).value<QNetworkConfiguration>();
    QVERIFY(config.isValid() == false);
    QVERIFY(player->currentNetworkConfiguration().isValid() == false);
}

void tst_QMediaPlayer::testSetVideoOutput()
{
    QVideoWidget widget;
    QGraphicsVideoItem item;
    MockVideoSurface surface;

    MockMediaPlayerService service;
    MockMediaServiceProvider provider(&service);
    QMediaPlayer player(0, 0, &provider);

    player.setVideoOutput(&widget);
    QVERIFY(widget.mediaObject() == &player);

    player.setVideoOutput(&item);
    QVERIFY(widget.mediaObject() == 0);
    QVERIFY(item.mediaObject() == &player);

    player.setVideoOutput(reinterpret_cast<QVideoWidget *>(0));
    QVERIFY(item.mediaObject() == 0);

    player.setVideoOutput(&widget);
    QVERIFY(widget.mediaObject() == &player);

    player.setVideoOutput(reinterpret_cast<QGraphicsVideoItem *>(0));
    QVERIFY(widget.mediaObject() == 0);

    player.setVideoOutput(&surface);
    QVERIFY(service.rendererControl->surface() == &surface);

    player.setVideoOutput(reinterpret_cast<QAbstractVideoSurface *>(0));
    QVERIFY(service.rendererControl->surface() == 0);

    player.setVideoOutput(&surface);
    QVERIFY(service.rendererControl->surface() == &surface);

    player.setVideoOutput(&widget);
    QVERIFY(service.rendererControl->surface() == 0);
    QVERIFY(widget.mediaObject() == &player);

    player.setVideoOutput(&surface);
    QVERIFY(service.rendererControl->surface() == &surface);
    QVERIFY(widget.mediaObject() == 0);
}


void tst_QMediaPlayer::testSetVideoOutputNoService()
{
    QVideoWidget widget;
    QGraphicsVideoItem item;
    MockVideoSurface surface;

    MockMediaServiceProvider provider(0, true);
    QMediaPlayer player(0, 0, &provider);

    player.setVideoOutput(&widget);
    QVERIFY(widget.mediaObject() == 0);

    player.setVideoOutput(&item);
    QVERIFY(item.mediaObject() == 0);

    player.setVideoOutput(&surface);
    // Nothing we can verify here other than it doesn't assert.
}

void tst_QMediaPlayer::testSetVideoOutputNoControl()
{
    QVideoWidget widget;
    QGraphicsVideoItem item;
    MockVideoSurface surface;

    MockMediaPlayerService service;
    service.rendererRef = 1;
    service.windowRef = 1;

    MockMediaServiceProvider provider(&service);
    QMediaPlayer player(0, 0, &provider);

    player.setVideoOutput(&widget);
    QVERIFY(widget.mediaObject() == 0);

    player.setVideoOutput(&item);
    QVERIFY(item.mediaObject() == 0);

    player.setVideoOutput(&surface);
    QVERIFY(service.rendererControl->surface() == 0);
}

void tst_QMediaPlayer::testSetVideoOutputDestruction()
{
    MockVideoSurface surface;

    MockMediaPlayerService service;
    MockMediaServiceProvider provider(&service);

    {
        QMediaPlayer player(0, 0, &provider);
        player.setVideoOutput(&surface);
        QVERIFY(service.rendererControl->surface() == &surface);
        QCOMPARE(service.rendererRef, 1);
    }
    QVERIFY(service.rendererControl->surface() == 0);
    QCOMPARE(service.rendererRef, 0);
}

void tst_QMediaPlayer::testPositionPropertyWatch()
{
    QMediaContent content0(QUrl(QLatin1String("test://audio/song1.mp3")));
    QMediaContent content1(QUrl(QLatin1String("test://audio/song2.mp3")));

    mockService->setIsValid(true);
    mockService->setState(QMediaPlayer::StoppedState, QMediaPlayer::NoMedia);

    QMediaPlaylist *playlist = new QMediaPlaylist;

    playlist->addMedia(content0);
    playlist->addMedia(content1);

    player->setPlaylist(playlist);
    player->setNotifyInterval(5);

    player->play();
    QSignalSpy positionSpy(player, SIGNAL(positionChanged(qint64)));
    playlist->next();
    QCOMPARE(player->state(), QMediaPlayer::PlayingState);
    QTRY_VERIFY(positionSpy.count() > 0);

    playlist->next();
    QCOMPARE(player->state(), QMediaPlayer::StoppedState);

    positionSpy.clear();
    QTRY_COMPARE(positionSpy.count(), 0);
}

void tst_QMediaPlayer::debugEnums()
{
    QTest::ignoreMessage(QtDebugMsg, "QMediaPlayer::PlayingState ");
    qDebug() << QMediaPlayer::PlayingState;
    QTest::ignoreMessage(QtDebugMsg, "QMediaPlayer::NoMedia ");
    qDebug() << QMediaPlayer::NoMedia;
    QTest::ignoreMessage(QtDebugMsg, "QMediaPlayer::NetworkError ");
    qDebug() << QMediaPlayer::NetworkError;
}

void tst_QMediaPlayer::testSupportedMimeTypes()
{
    QStringList mimeList = QMediaPlayer::supportedMimeTypes(QMediaPlayer::LowLatency);

    // This is empty on some platforms, and not on others, so can't test something here at the moment.
}
