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

#include <qmediaresource.h>

#include <QtCore/qsize.h>
#include <QtCore/qurl.h>
#include <QtCore/qvariant.h>

QTM_BEGIN_NAMESPACE

/*!
    \class QMediaResource
    \preliminary
    \brief The QMediaResource class provides a description of a media resource.

    A media resource is composed of a \l {uri()}{URI} containing the location of the resource and a
    set of properties that describe the format of the resource.  The properties provide a means to
    assess a resource without first attempting to load it, and in situations where media be
    represented by multiple alternative representations provide a means to select the appropriate
    resource.

    Media made available by a remote services can often be available in multiple encodings or
    quality levels, this allows a client to select an appropriate resource based on considerations
    such as codecs supported, network bandwidth, and display constraints.  QMediaResource includes
    information such as the \l {mimeType()}{MIME type}, \l {audioCodec()}{audio} and
    \l {videoCodec()}{video} codecs, \l {audioBitRate()}{audio} and \l {videoBitRate()}{video} bit
    rates, and \l {resolution()}{resolution} so these constraints and others can be evaluated.

    The only mandatory property of a QMediaResource is the uri().

    \sa QMediaContent
*/

/*!
    \typedef QMediaResourceList

    Synonym for \c QList<QMediaResource>
*/

/*!
    Constructs a null media resource.
*/
QMediaResource::QMediaResource()
{
}

/*!
    Constructs a media resource with the given \a mimeType from a \a uri.
*/
QMediaResource::QMediaResource(const QUrl &uri, const QString &mimeType)
{
    values.insert(Uri, qVariantFromValue(uri));
    values.insert(MimeType, mimeType);
}

/*!
    Constructs a copy of a media resource \a other.
*/
QMediaResource::QMediaResource(const QMediaResource &other)
    : values(other.values)
{
}

/*!
    Assigns the value of \a other to a media resource.
*/
QMediaResource &QMediaResource::operator =(const QMediaResource &other)
{
    values = other.values;

    return *this;
}

/*!
    Destroys a media resource.
*/
QMediaResource::~QMediaResource()
{
}


/*!
    Compares a media resource to \a other.

    Returns true if the resources are identical, and false otherwise.
*/
bool QMediaResource::operator ==(const QMediaResource &other) const
{
    return values == other.values;
}

/*!
    Compares a media resource to \a other.

    Returns true if they are different, and false otherwise.
*/
bool QMediaResource::operator !=(const QMediaResource &other) const
{
    return values != other.values;
}

/*!
    Identifies if a media resource is null.

    Returns true if the resource is null, and false otherwise.
*/
bool QMediaResource::isNull() const
{
    return values.isEmpty();
}

/*!
    Returns the URI of a media resource.
*/
QUrl QMediaResource::uri() const
{
    return qvariant_cast<QUrl>(values.value(Uri));
}

/*!
    Returns the MIME type of a media resource.

    This may be null if the MIME type is unknown.
*/
QString QMediaResource::mimeType() const
{
    return qvariant_cast<QString>(values.value(MimeType));
}

/*!
    Returns the language of a media resource as an ISO 639-2 code.

    This may be null if the language is unknown.
*/
QString QMediaResource::language() const
{
    return qvariant_cast<QString>(values.value(Language));
}

/*!
    Sets the \a language of a media resource.
*/
void QMediaResource::setLanguage(const QString &language)
{
    if (!language.isNull())
        values.insert(Language, language);
    else
        values.remove(Language);
}

/*!
    Returns the audio codec of a media resource.

    This may be null if the media resource does not contain an audio stream, or the codec is
    unknown.
*/
QString QMediaResource::audioCodec() const
{
    return qvariant_cast<QString>(values.value(AudioCodec));
}

/*!
    Sets the audio \a codec of a media resource.
*/
void QMediaResource::setAudioCodec(const QString &codec)
{
    if (!codec.isNull())
        values.insert(AudioCodec, codec);
    else
        values.remove(AudioCodec);
}

/*!
    Returns the video codec of a media resource.

    This may be null if the media resource does not contain a video stream, or the codec is
    unknonwn.
*/
QString QMediaResource::videoCodec() const
{
    return qvariant_cast<QString>(values.value(VideoCodec));
}

/*!
    Sets the video \a codec of media resource.
*/
void QMediaResource::setVideoCodec(const QString &codec)
{
    if (!codec.isNull())
        values.insert(VideoCodec, codec);
    else
        values.remove(VideoCodec);
}

/*!
    Returns the size in bytes of a media resource.

    This may be zero if the size is unknown.
*/
qint64 QMediaResource::size() const
{
    return qvariant_cast<qint64>(values.value(Size));
}

/*!
    Sets the \a size in bytes of a media resource.
*/
void QMediaResource::setSize(const qint64 size)
{
    if (size != 0)
        values.insert(Size, size);
    else
        values.remove(Size);
}

/*!
    Returns the duration in milliseconds of a media resource.

    This may be zero if the duration is unknown, or the resource has no explicit duration (i.e. the
    resource is an image, or a live stream).
*/
qint64 QMediaResource::duration() const
{
    return qvariant_cast<qint64>(values.value(Duration));
}

/*!
    Sets the \a duration in milliseconds of a media resource.
*/
void QMediaResource::setDuration(qint64 duration)
{
    if (duration != 0)
        values.insert(Duration, duration);
    else
        values.remove(Duration);
}

/*!
    Returns the bit rate in bits per second of a media resource's audio stream.

    This may be zero if the bit rate is unknown, or the resource contains no audio stream.
*/
int QMediaResource::audioBitRate() const
{
    return values.value(AudioBitRate).toInt();
}

/*!
    Sets the bit \a rate in bits per second of a media resource's video stream.
*/
void QMediaResource::setAudioBitRate(int rate)
{
    if (rate != 0)
        values.insert(AudioBitRate, rate);
    else
        values.remove(AudioBitRate);
}

/*!
    Returns the audio sample size in bits per sample of a media resource.

    This may return zero if the sample size is unknown, or the resource contains no audio stream.
*/
int QMediaResource::sampleSize() const
{
    return qvariant_cast<int>(values.value(SampleSize));
}

/*!
    Sets the audio sample \a size of a media resource.
*/
void QMediaResource::setSampleSize(int size)
{
    if (size != 0)
        values.insert(SampleSize, size);
    else
        values.remove(SampleSize);
}

/*!
    Returns the audio sample frequency of a media resource.

    This may be zero if the sample size is unknown, or the resource contains no audio stream.
*/
int QMediaResource::frequency() const
{
    return qvariant_cast<int>(values.value(Frequency));
}

/*!
    Sets the audio sample \a frequency of a media resource.
*/
void QMediaResource::setFrequency(int frequency)
{
    if (frequency != 0)
        values.insert(Frequency, frequency);
    else
        values.remove(Frequency);
}

/*!
    Returns the number of audio channels in a media resource.

    This may be zero if the sample size is unknown, or the resource contains no audio stream.
*/
int QMediaResource::channels() const
{
    return qvariant_cast<int>(values.value(Channels));
}

/*!
    Sets the number of audio \a channels in a media resource.
*/
void QMediaResource::setChannels(int channels)
{
    if (channels != 0)
        values.insert(Channels, channels);
    else
        values.remove(Channels);
}

/*!
    Returns the bit rate in bits per second of a media resource's video stream.

    This may be zero if the bit rate is unknown, or the resource contains no video stream.
*/
int QMediaResource::videoBitRate() const
{
    return values.value(VideoBitRate).toInt();
}

/*!
    Sets the bit \a rate in bits per second of a media resource's video stream.
*/
void QMediaResource::setVideoBitRate(int rate)
{
    if (rate != 0)
        values.insert(VideoBitRate, rate);
    else
        values.remove(VideoBitRate);
}

/*!
    Returns the resolution in pixels of a media resource.

    This may be null is the resolution is unknown, or the resource contains no pixel data (i.e. the
    resource is an audio stream.
*/
QSize QMediaResource::resolution() const
{
    return qvariant_cast<QSize>(values.value(Resolution));
}

/*!
    Sets the \a resolution in pixels of a media resource.
*/
void QMediaResource::setResolution(const QSize &resolution)
{
    if (resolution.width() != -1 || resolution.height() != -1)
        values.insert(Resolution, resolution);
    else
        values.remove(Resolution);
}

/*!
    Sets the \a width and \a height in pixels of a media resource.
*/
void QMediaResource::setResolution(int width, int height)
{
    if (width != -1 || height != -1)
        values.insert(Resolution, QSize(width, height));
    else
        values.remove(Resolution);
}
QTM_END_NAMESPACE
