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

#ifndef QMEDIARECORDER_H
#define QMEDIARECORDER_H

#include <qmediaobject.h>
#include <qmediaserviceprovider.h>
#include <qmediaencodersettings.h>
#include <qmediabindableinterface.h>
#include <qmediaenumdebug.h>

#include <QtCore/qpair.h>

QT_BEGIN_NAMESPACE
class QUrl;
class QSize;
class QAudioFormat;
QT_END_NAMESPACE

QT_BEGIN_NAMESPACE

class QMediaRecorderService;
class QAudioEncoderSettings;
class QVideoEncoderSettings;

class QMediaRecorderPrivate;
class Q_MULTIMEDIA_EXPORT QMediaRecorder : public QObject, public QMediaBindableInterface
{
    Q_OBJECT
    Q_INTERFACES(QMediaBindableInterface)
    Q_ENUMS(State)
    Q_ENUMS(Error)
    Q_PROPERTY(qint64 duration READ duration NOTIFY durationChanged)
    Q_PROPERTY(QUrl outputLocation READ outputLocation WRITE setOutputLocation)
    Q_PROPERTY(bool muted READ isMuted WRITE setMuted NOTIFY mutedChanged)
    Q_PROPERTY(bool metaDataAvailable READ isMetaDataAvailable NOTIFY metaDataAvailableChanged)
    Q_PROPERTY(bool metaDataWritable READ isMetaDataWritable NOTIFY metaDataWritableChanged)
public:

    enum State
    {
        StoppedState,
        RecordingState,
        PausedState
    };

    enum Error
    {
        NoError,
        ResourceError,
        FormatError
    };

    QMediaRecorder(QMediaObject *mediaObject, QObject *parent = 0);
    ~QMediaRecorder();

    QMediaObject *mediaObject() const;

    bool isAvailable() const;
    QtMultimediaKit::AvailabilityError availabilityError() const;

    QUrl outputLocation() const;
    bool setOutputLocation(const QUrl &location);

    State state() const;

    Error error() const;
    QString errorString() const;

    qint64 duration() const;

    bool isMuted() const;

    QStringList supportedContainers() const;
    QString containerDescription(const QString &containerMimeType) const;

    QStringList supportedAudioCodecs() const;
    QString audioCodecDescription(const QString &codecName) const;

    QList<int> supportedAudioSampleRates(const QAudioEncoderSettings &settings = QAudioEncoderSettings(),
                                         bool *continuous = 0) const;

    QStringList supportedVideoCodecs() const;
    QString videoCodecDescription(const QString &codecName) const;

    QList<QSize> supportedResolutions(const QVideoEncoderSettings &settings = QVideoEncoderSettings(),
                                      bool *continuous = 0) const;

    QList<qreal> supportedFrameRates(const QVideoEncoderSettings &settings = QVideoEncoderSettings(),
                                     bool *continuous = 0) const;

    QAudioEncoderSettings audioSettings() const;
    QVideoEncoderSettings videoSettings() const;
    QString containerMimeType() const;

    void setEncodingSettings(const QAudioEncoderSettings &audioSettings,
                             const QVideoEncoderSettings &videoSettings = QVideoEncoderSettings(),
                             const QString &containerMimeType = QString());


    bool isMetaDataAvailable() const;
    bool isMetaDataWritable() const;

    QVariant metaData(QtMultimediaKit::MetaData key) const;
    void setMetaData(QtMultimediaKit::MetaData key, const QVariant &value);
    QList<QtMultimediaKit::MetaData> availableMetaData() const;

    QVariant extendedMetaData(const QString &key) const;
    void setExtendedMetaData(const QString &key, const QVariant &value);
    QStringList availableExtendedMetaData() const;

public Q_SLOTS:
    void record();
    void pause();
    void stop();
    void setMuted(bool muted);

Q_SIGNALS:
    void stateChanged(QMediaRecorder::State state);
    void durationChanged(qint64 duration);
    void mutedChanged(bool muted);

    void error(QMediaRecorder::Error error);

    void metaDataAvailableChanged(bool available);
    void metaDataWritableChanged(bool writable);
    void metaDataChanged();

protected:
    bool setMediaObject(QMediaObject *object);

private:
    QMediaRecorderPrivate *d_ptr;
    Q_DISABLE_COPY(QMediaRecorder)
    Q_DECLARE_PRIVATE(QMediaRecorder)
    Q_PRIVATE_SLOT(d_func(), void _q_stateChanged(QMediaRecorder::State))
    Q_PRIVATE_SLOT(d_func(), void _q_error(int, const QString &))
    Q_PRIVATE_SLOT(d_func(), void _q_serviceDestroyed())
    Q_PRIVATE_SLOT(d_func(), void _q_notify())
    Q_PRIVATE_SLOT(d_func(), void _q_updateNotifyInterval(int))
};

QT_END_NAMESPACE

Q_DECLARE_METATYPE(QMediaRecorder::State)
Q_DECLARE_METATYPE(QMediaRecorder::Error)

Q_MEDIA_ENUM_DEBUG(QMediaRecorder, State)
Q_MEDIA_ENUM_DEBUG(QMediaRecorder, Error)

#endif  // QMEDIARECORDER_H
