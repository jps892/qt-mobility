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

#ifndef QMEDIAIMAGEVIEWER_H
#define QMEDIAIMAGEVIEWER_H

#include <qmediaobject.h>
#include <qmediacontent.h>

QTM_BEGIN_NAMESPACE


class QMediaImageViewerPrivate;
class Q_MEDIA_EXPORT QMediaImageViewer : public QMediaObject
{
    Q_OBJECT
    Q_PROPERTY(State state READ state NOTIFY stateChanged)
    Q_PROPERTY(MediaStatus mediaStatus READ mediaStatus NOTIFY mediaStatusChanged)
    Q_PROPERTY(QMediaContent media READ media WRITE setMedia NOTIFY mediaChanged)
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout)
    Q_PROPERTY(int elapsedTime READ elapsedTime NOTIFY elapsedTimeChanged)
    Q_ENUMS(State MediaStatus)

public:
    enum State
    {
        StoppedState,
        PlayingState,
        PausedState
    };

    enum MediaStatus
    {
        NoMedia,
        LoadingMedia,
        LoadedMedia,
        InvalidMedia
    };

    explicit QMediaImageViewer(QObject *parent = 0);
    ~QMediaImageViewer();

    State state() const;
    MediaStatus mediaStatus() const;

    QMediaContent media() const;

    int timeout() const;
    int elapsedTime() const;

    void bind(QObject *);

public Q_SLOTS:
    void setMedia(const QMediaContent &media);

    void play();
    void pause();
    void stop();

    void setTimeout(int timeout);

Q_SIGNALS:
    void stateChanged(QMediaImageViewer::State state);
    void mediaStatusChanged(QMediaImageViewer::MediaStatus status);
    void mediaChanged(const QMediaContent &media);
    void elapsedTimeChanged(int time);
protected:
    void timerEvent(QTimerEvent *event);

private:
    Q_DECLARE_PRIVATE(QMediaImageViewer)
    Q_PRIVATE_SLOT(d_func(), void _q_mediaStatusChanged(QMediaImageViewer::MediaStatus))
    Q_PRIVATE_SLOT(d_func(), void _q_playlistMediaChanged(const QMediaContent &))
    Q_PRIVATE_SLOT(d_func(), void _q_playlistDestroyed())
};

QTM_END_NAMESPACE

#endif