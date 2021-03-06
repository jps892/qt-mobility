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

#ifndef QLANDMARKREQUESTHANDLER_H_
#define QLANDMARKREQUESTHANDLER_H_

// system includes
#include <EPos_CPosLandmarkSearch.h>
#include <EPos_Landmarks.h>
#include <f32file.h>  
#include <qmobilityglobal.h>
#include <qstring.h>
#include <qset.h>
#include <qlandmarkrequesthandler.h>
#include <qlandmarkmanagerengine.h>

QTM_USE_NAMESPACE
typedef QMap<int, QLandmarkManager::Error> ERROR_MAP;

class CLandmarkRequestAO;
class CLandmarkRequestData;
class LandmarkRequestHandler;
class LandmarkManagerEngineSymbianPrivate;

/***
 * 
 */
class TWaitThrdData
{
public:
    TInt iThrdId;
    TRequestStatus iThrdRequest;
private:
    TSglQueLink iLink;
public:
    TWaitThrdData();
    friend class CLandmarkRequestData;
};

/***
 * 
 */
class CLandmarkRequestData: public CBase
{
public:
    static CLandmarkRequestData
    * NewL(QLandmarkAbstractRequest *, CLandmarkRequestAO *);
    virtual ~CLandmarkRequestData();
    void Reset();
private:
    CLandmarkRequestData(QLandmarkAbstractRequest *, CLandmarkRequestAO *);
    void ConstructL();

private:
    // for AO
    CLandmarkRequestAO *iOwnerAO;
    TInt iOwnerThrd;
    TSglQue<TWaitThrdData> iWaitThrds;
    RFastLock iLock;

    // for engine
    // not owned
    QLandmarkAbstractRequest *iQtRequest;

    // owned
    CPosLandmarkSearch *iLandmarkSearch;
    CPosLandmarkEncoder *iLandmarkEncoder;
    RFs iFileSystem;
    CBufBase *iExportBuffer;
    HBufC *iExportPath;
    QList<QLandmarkCategoryId> iCategoryIds;
    QList<QLandmarkId> iLandmarkIds;
    QList<QLandmark> iLandmarks;
    QList<QLandmarkCategory> iCategories;
    QSet<QString> iLocalIds;

    QList<QLandmarkId> iAddedLandmarkIds;
    QList<QLandmarkId> iChangedLandmarkIds;
    QList<QLandmarkId> iRemovedLandmarkIds;
    QList<QLandmarkId> iFetchedLandmarkIds;

    QList<QLandmarkCategoryId> iAddedCategoryIds;
    QList<QLandmarkCategoryId> iChangedCategoryIds;
    QList<QLandmarkCategoryId> iRemovedCategoryIds;
    QList<QLandmarkCategoryId> iFetchedCategoryIds;

    ERROR_MAP iErrorMap;
    TInt iErrorId;
    QLandmarkManager::Error error;
    QString errorString;
    int iOpCount;
    int iFilterIndex;

private:
    friend class CLandmarkRequestAO;
    friend class LandmarkRequestHandler;
    friend class LandmarkManagerEngineSymbianPrivate;
};

/***
 * 
 */
class MLandmarkRequestObserver
{
public:
    virtual void HandleExecutionL(CLandmarkRequestData *, TRequestStatus &) = 0;
    virtual void HandleCompletionL(CLandmarkRequestData *) = 0;
};

/***
 * 
 */
class CLandmarkRequestAO: public CActive
{
public:
    TBool StartRequest(CPosLandmarkSearch *aLandmarkSearch);
    TBool CancelRequest();
    TBool WaitForFinished(TInt);
    void SetOperation(CPosLmOperation *aOp = NULL);
    void SetExportData(CPosLandmarkEncoder *aEncoder, RFs &aFileSystem, HBufC *aExportPath,
        CBufBase *aExportBuffer, QList<QLandmarkId> lmIds = QList<QLandmarkId> ());
    CPosLmOperation * GetOperation();
    static CLandmarkRequestAO* NewL(MLandmarkRequestObserver *, CPosLmOperation *aOp = NULL);
    inline void SetParent(CLandmarkRequestData *aData)
    {
        iParent = aData;
    }
    virtual ~CLandmarkRequestAO();

private:
    CLandmarkRequestAO(CPosLmOperation *, MLandmarkRequestObserver *);
    void RunL();
    TInt RunError(TInt aError);
    void DoCancel();
    void WakeupThreads(TInt);
    TBool WaitForRequestL(TInt, TRequestStatus &);
    TBool IsMultiOperationRequest();
private:
    MLandmarkRequestObserver *iObserver;
    CPosLmOperation *iOperation;
    CLandmarkRequestData *iParent;
    TBool iCancelRequest;
    TReal32 iProgress;
    TBool iIsComplete;
    TBool iIsRequestRunning;
    TBool iIsDestructing;
    TBool iIsWFFRequestComplete;
};

/**
 * 
 */
class LandmarkRequestHandler
{
public:
    LandmarkRequestHandler();
    ~LandmarkRequestHandler();
    TInt AddAsyncRequest(QLandmarkAbstractRequest *, CLandmarkRequestAO *aAO);
    TBool RemoveAsyncRequest(QLandmarkAbstractRequest *);
    CLandmarkRequestData* FetchAsyncRequest(QLandmarkAbstractRequest *);

private:
    RPointerArray<CLandmarkRequestData> iRequestList;
    RFastLock iRequestListLock;
};

#endif /* QLANDMARKREQUESTHANDLER_H_ */
