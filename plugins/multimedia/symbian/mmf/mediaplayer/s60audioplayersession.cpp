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

#include "s60audioplayersession.h"
#include "s60mmtrace.h"

S60AudioPlayerSession::S60AudioPlayerSession(QObject *parent)
    : S60MediaPlayerSession(parent)
    , m_player(0)
    , m_audioEndpoint("Default")
{
    TRACE("S60AudioPlayerSession::S60AudioPlayerSession" << qtThisPtr());
#ifdef HAS_AUDIOROUTING
    m_audioOutput = 0;
#endif
    QT_TRAP_THROWING(m_player = CAudioPlayer::NewL(*this, 0, EMdaPriorityPreferenceNone));
    m_player->RegisterForAudioLoadingNotification(*this);
}

S60AudioPlayerSession::~S60AudioPlayerSession()
{
    TRACE("S60AudioPlayerSession::~S60AudioPlayerSession" << qtThisPtr());
#ifdef HAS_AUDIOROUTING
    if (m_audioOutput)
        m_audioOutput->UnregisterObserver(*this);
    delete m_audioOutput;
#endif
    m_player->Close();
    delete m_player;
}

void S60AudioPlayerSession::doLoadL(const TDesC &path)
{
#ifdef HAS_AUDIOROUTING
    // m_audioOutput needs to be reinitialized after MapcInitComplete
    if (m_audioOutput)
        m_audioOutput->UnregisterObserver(*this);
    delete m_audioOutput;
    m_audioOutput = 0;
#endif
    m_player->OpenFileL(path);
}

void S60AudioPlayerSession::doLoadUrlL(const TDesC &path)
{
    Q_UNUSED(path);
}

qint64 S60AudioPlayerSession::doGetDurationL() const
{
    return m_player->Duration().Int64() / qint64(1000);
}

qint64 S60AudioPlayerSession::doGetPositionL() const
{
    TTimeIntervalMicroSeconds ms = 0;
    m_player->GetPosition(ms);
    return ms.Int64() / qint64(1000);
}

bool S60AudioPlayerSession::isVideoAvailable()
{
    return false;
}

bool S60AudioPlayerSession::isAudioAvailable()
{
    return true;
}

void S60AudioPlayerSession::MaloLoadingStarted()
{
    TRACE("S60AudioPlayerSession::MaloLoadingStarted" << qtThisPtr());
    buffering();
}

void S60AudioPlayerSession::MaloLoadingComplete()
{
    TRACE("S60AudioPlayerSession::MaloLoadingComplete" << qtThisPtr());
    buffered();
}

void S60AudioPlayerSession::doPlay()
{
    buffering();
    m_player->Play();
    buffered();
}

void S60AudioPlayerSession::doPauseL()
{
    m_player->Pause();
}

void S60AudioPlayerSession::doStop()
{
    m_player->Stop();
}

void S60AudioPlayerSession::doClose()
{
#ifdef HAS_AUDIOROUTING
    if (m_audioOutput) {
        m_audioOutput->UnregisterObserver(*this);
        delete m_audioOutput;
        m_audioOutput = 0;
    }
#endif
    m_player->Close();
}

void S60AudioPlayerSession::doSetVolumeL(int volume)
{
    m_player->SetVolume(volume * m_player->MaxVolume() / 100);
}

void S60AudioPlayerSession::doSetPositionL(qint64 microSeconds)
{
    m_player->SetPosition(TTimeIntervalMicroSeconds(microSeconds));
}

void S60AudioPlayerSession::updateMetaDataEntriesL()
{
    metaDataEntries().clear();
    int numberOfMetaDataEntries = 0;
    m_player->GetNumberOfMetaDataEntries(numberOfMetaDataEntries);
    for (int i=0; i<numberOfMetaDataEntries; ++i) {
        CMMFMetaDataEntry *entry = 0;
        entry = m_player->GetMetaDataEntryL(i);
        metaDataEntries().insert(QString::fromUtf16(entry->Name().Ptr(), entry->Name().Length()),
                                 QString::fromUtf16(entry->Value().Ptr(), entry->Value().Length()));
        delete entry;
    }
    emit metaDataChanged();
}

void S60AudioPlayerSession::setPlaybackRate(qreal rate)
{
    TRACE("S60AudioPlayerSession::setPlaybackRate" << qtThisPtr() << "rate" << rate);
    if (rate == 1.0) {
        emit playbackRateChanged(rate);
        return;
    } else {
        int err = KErrNotSupported;
        setAndEmitError(err);
    }
}

int S60AudioPlayerSession::doGetBufferStatusL() const
{
    int progress = 0;
    m_player->GetAudioLoadingProgressL(progress);
    return progress;
}

#ifdef S60_DRM_SUPPORTED
void S60AudioPlayerSession::MdapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration)
#else
void S60AudioPlayerSession::MapcInitComplete(TInt aError, const TTimeIntervalMicroSeconds& aDuration)
#endif
{
    TRACE("S60AudioPlayerSession::MapcInitComplete" << qtThisPtr() << "error" << aError);
    Q_UNUSED(aDuration);
    setError(aError);
    if (KErrNone != aError)
        return;
#ifdef HAS_AUDIOROUTING    
    TRAPD(err, 
          m_audioOutput = CAudioOutput::NewL(*m_player);
          m_audioOutput->RegisterObserverL(*this);
    );
    setActiveEndpoint(m_audioEndpoint);
    setError(err);
#endif //HAS_AUDIOROUTING
    if (KErrNone == aError)
        loaded();
}

#ifdef S60_DRM_SUPPORTED
void S60AudioPlayerSession::MdapcPlayComplete(TInt aError)
#else
void S60AudioPlayerSession::MapcPlayComplete(TInt aError)
#endif
{
    TRACE("S60AudioPlayerSession::MapcPlayComplete" << qtThisPtr() << "error" << aError);
    if (KErrNone == aError)
        endOfMedia();
    else
        setError(aError);
}

void S60AudioPlayerSession::doSetAudioEndpoint(const QString &audioEndpoint)
{
    m_audioEndpoint = audioEndpoint;
}

QString S60AudioPlayerSession::activeEndpoint() const
{
    QString outputName = QString("Default");
#ifdef HAS_AUDIOROUTING
    if (m_audioOutput) {
        CAudioOutput::TAudioOutputPreference output = m_audioOutput->AudioOutput();
        outputName = qStringFromTAudioOutputPreference(output);
    }
#endif
    return outputName;
}

QString S60AudioPlayerSession::defaultEndpoint() const
{
    QString outputName = QString("Default");
#ifdef HAS_AUDIOROUTING
    if (m_audioOutput) {
        CAudioOutput::TAudioOutputPreference output = m_audioOutput->DefaultAudioOutput();
        outputName = qStringFromTAudioOutputPreference(output);
    }
#endif
    return outputName;
}

void S60AudioPlayerSession::setActiveEndpoint(const QString &name)
{
    TRACE("S60AudioPlayerSession::setActiveEndpoint" << qtThisPtr() << "name" << name);
#ifdef HAS_AUDIOROUTING
    CAudioOutput::TAudioOutputPreference output = CAudioOutput::ENoPreference;
    if (name == QString("Default"))
        output = CAudioOutput::ENoPreference;
    else if (name == QString("All"))
        output = CAudioOutput::EAll;
    else if (name == QString("None"))
        output = CAudioOutput::ENoOutput;
    else if (name == QString("Earphone"))
        output = CAudioOutput::EPrivate;
    else if (name == QString("Speaker"))
        output = CAudioOutput::EPublic;
    if (m_audioOutput) {
        TRAPD(err, m_audioOutput->SetAudioOutputL(output));
        setError(err);
    }
#endif
}

#ifdef HAS_AUDIOROUTING
void S60AudioPlayerSession::DefaultAudioOutputChanged(CAudioOutput& aAudioOutput,
                                                      CAudioOutput::TAudioOutputPreference aNewDefault)
{
    TRACE("S60AudioPlayerSession::DefaultAudioOutputChanged" << qtThisPtr()
          << "newDefault" << aNewDefault);
    // Emit already implemented in setActiveEndpoint function
    Q_UNUSED(aAudioOutput)
    Q_UNUSED(aNewDefault)
}

QString S60AudioPlayerSession::qStringFromTAudioOutputPreference(CAudioOutput::TAudioOutputPreference output) const
{
    if (output == CAudioOutput::ENoPreference)
        return QString("Default");
    else if (output == CAudioOutput::EAll)
        return QString("All");
    else if (output == CAudioOutput::ENoOutput)
        return QString("None");
    else if (output == CAudioOutput::EPrivate)
        return QString("Earphone");
    else if (output == CAudioOutput::EPublic)
        return QString("Speaker");
    return QString("Default");
}
#endif

bool S60AudioPlayerSession::getIsSeekable() const
{
    return true;
}
