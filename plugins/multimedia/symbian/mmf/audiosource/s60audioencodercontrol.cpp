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

#include "s60audioencodercontrol.h"
#include "s60audiocapturesession.h"
#include "s60mmtrace.h"
#include "qaudioformat.h"

S60AudioEncoderControl::S60AudioEncoderControl(QObject *session, QObject *parent)
    : QAudioEncoderControl(parent), m_quality(QtMultimediaKit::NormalQuality)
{
    TRACE("S60AudioEncoderControl::S60AudioEncoderControl" << qtThisPtr());
    m_session = qobject_cast<S60AudioCaptureSession*>(session);
    QAudioFormat fmt = m_session->format();
    // medium, 22050Hz mono S16
    fmt.setSampleType(QAudioFormat::SignedInt);
    if (fmt.codec().compare("PCM", Qt::CaseInsensitive) == 0) {
        fmt.setSampleSize(16);
        fmt.setFrequency(22050);
    }
    fmt.setChannels(1);
    m_session->setFormat(fmt);
    m_settings.setChannelCount(fmt.channels());
    m_settings.setCodec(fmt.codec());
    m_settings.setSampleRate(fmt.sampleRate());
}

S60AudioEncoderControl::~S60AudioEncoderControl()
{
    TRACE("S60AudioEncoderControl::~S60AudioEncoderControl" << qtThisPtr());
}

QStringList S60AudioEncoderControl::supportedAudioCodecs() const
{
    return m_session->supportedAudioCodecs();
}

QString S60AudioEncoderControl::codecDescription(const QString &codecName) const
{
    return m_session->codecDescription(codecName);
}

QtMultimediaKit::EncodingQuality S60AudioEncoderControl::quality() const
{
    return m_quality;
}

void S60AudioEncoderControl::setQuality(QtMultimediaKit::EncodingQuality value, QAudioFormat &fmt)
{
    TRACE("S60AudioEncoderControl::setQuality" << qtThisPtr() << "value" << value);

    switch (value) {
    case QtMultimediaKit::VeryLowQuality:
    case QtMultimediaKit::LowQuality:
        // low, 8000Hz mono U8
        fmt.setSampleType(QAudioFormat::UnSignedInt);
        fmt.setSampleSize(8);
        fmt.setFrequency(8000);
        fmt.setChannels(1);
        break;
    case QtMultimediaKit::NormalQuality:
        // medium, 22050Hz mono S16
        fmt.setSampleType(QAudioFormat::SignedInt);
        fmt.setSampleSize(16);
        fmt.setFrequency(22050);
        fmt.setChannels(1);
        break;
    case QtMultimediaKit::HighQuality:
    case QtMultimediaKit::VeryHighQuality:
        // high, 44100Hz mono S16
        fmt.setSampleType(QAudioFormat::SignedInt);
        fmt.setSampleSize(16);
        fmt.setFrequency(44100);
        fmt.setChannels(2);
        break;
    default:
        break;
    }
}

QStringList S60AudioEncoderControl::supportedEncodingOptions(const QString &codec) const
{
    Q_UNUSED(codec)
    QStringList list;
    if (codec == "PCM") 
        list << "quality" << "channels" << "samplerate";        
    return list;
}

QVariant S60AudioEncoderControl::encodingOption(const QString &codec, const QString &name) const
{
    if (codec == "PCM") {
        QAudioFormat fmt = m_session->format();
        if(qstrcmp(name.toLocal8Bit().constData(), "quality") == 0)
            return QVariant(quality());
        else if(qstrcmp(name.toLocal8Bit().constData(), "channels") == 0)
            return QVariant(fmt.channels());
        else if(qstrcmp(name.toLocal8Bit().constData(), "samplerate") == 0)
            return QVariant(fmt.frequency());
    }
    return QVariant();
}

void S60AudioEncoderControl::setEncodingOption(
        const QString &codec, const QString &name, const QVariant &value)
{
    TRACE("S60AudioEncoderControl::setEncodingOption" << qtThisPtr()
          << "codec" << codec << "name" << name << "value" << value);
    if (codec == "PCM") {
        QAudioFormat fmt = m_session->format();
        if(qstrcmp(name.toLocal8Bit().constData(), "quality") == 0)
            setQuality((QtMultimediaKit::EncodingQuality)value.toInt(), fmt);
        else if(qstrcmp(name.toLocal8Bit().constData(), "channels") == 0)
            fmt.setChannels(value.toInt());
        else if(qstrcmp(name.toLocal8Bit().constData(), "samplerate") == 0)
            fmt.setFrequency(value.toInt());
        m_session->setFormat(fmt);
    }
}

QList<int> S60AudioEncoderControl::supportedSampleRates(const QAudioEncoderSettings &settings, bool *continuous) const
{
    if (continuous)
        *continuous = false;
    return m_session->supportedAudioSampleRates(settings);       
}

QAudioEncoderSettings S60AudioEncoderControl::audioSettings() const
{
    return m_settings;
}

void S60AudioEncoderControl::setAudioSettings(const QAudioEncoderSettings &settings)
{
    TRACE("S60AudioEncoderControl::setAudioSettings" << qtThisPtr()
          << "encodingMode" << settings.encodingMode()
          << "codec" << settings.codec()
          << "bitRate" << settings.bitRate()
          << "channelCount" << settings.channelCount()
          << "sampleRate" << settings.sampleRate()
          << "quality" << settings.quality());
    QAudioFormat fmt = m_session->format();
    if (settings.encodingMode() == QtMultimediaKit::ConstantQualityEncoding) {
        fmt.setCodec(settings.codec());
        setQuality(settings.quality(), fmt);
        if (settings.sampleRate() > 0)
            fmt.setFrequency(settings.sampleRate());
        if (settings.channelCount() > 0)
            fmt.setChannels(settings.channelCount());
    } else {
        if (settings.sampleRate() == 8000) {
            fmt.setSampleType(QAudioFormat::UnSignedInt);
            fmt.setSampleSize(8);
        } else {
            fmt.setSampleType(QAudioFormat::SignedInt);
            fmt.setSampleSize(16);
        }
        fmt.setCodec(settings.codec());
        fmt.setFrequency(settings.sampleRate());
        fmt.setChannels(settings.channelCount());
    }
    m_session->setFormat(fmt);
    m_session->setEncoderSettings(settings);
    m_settings = settings;
}
