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

#ifndef REQUESTEXAMPLE_H
#define REQUESTEXAMPLE_H

#include <QObject>
#include "qlandmarkfetchrequest.h"
#include "qlandmarkcategorysaverequest.h"
#include "qlandmarksaverequest.h"
#include "qlandmarkcategoryfetchrequest.h"
#include "qlandmarkcategoryremoverequest.h"
#include "qlandmarkremoverequest.h"
#include "qlandmarkimportrequest.h"
#include "qlandmarkexportrequest.h"
#include "qlandmarkmanager.h"


QTM_USE_NAMESPACE

class RequestExample : public QObject
{
    Q_OBJECT
public:
    RequestExample():QObject(),
                    lmManager(new QLandmarkManager(this)),
                    catSaveRequest(new QLandmarkCategorySaveRequest(lmManager, this)),
                    lmSaveRequest(new QLandmarkSaveRequest(lmManager, this)),
                    catFetchRequest(new QLandmarkCategoryFetchRequest(lmManager, this)),
                    lmFetchRequest(new QLandmarkFetchRequest(lmManager, this)),
                    catRemoveRequest(new QLandmarkCategoryRemoveRequest(lmManager, this)),
                    lmRemoveRequest(new QLandmarkRemoveRequest(lmManager, this))
                    {}
    ~RequestExample(){}

private slots:
    void categorySaveRequest();
    void categorySaveRequestHandler(QLandmarkAbstractRequest::State);
    void landmarkSaveRequest();
    void landmarkSaveRequestHandler(QLandmarkAbstractRequest::State);
    void categoryFetchRequest();
    void categoryFetchRequestHandler(QLandmarkAbstractRequest::State);
    void landmarkFetchRequest();
    void landmarkFetchRequestHandler(QLandmarkAbstractRequest::State);
    void categoryRemoveRequest();
    void categoryRemoveRequestHandler(QLandmarkAbstractRequest::State);
    void landmarkRemoveRequest();
    void landmarkRemoveRequestHandler(QLandmarkAbstractRequest::State);
    void landmarkImportRequest();
    void landmarkImportRequestHandler(QLandmarkAbstractRequest::State);
    void landmarkExportRequest();
    void landmarkExportRequestHandler(QLandmarkAbstractRequest::State);

private:
    QLandmarkManager *lmManager;
    QLandmarkFetchRequest *m_fetchRequest;
    QLandmarkCategorySaveRequest *catSaveRequest;
    QLandmarkSaveRequest *lmSaveRequest;
    QLandmarkCategoryFetchRequest *catFetchRequest;
    QLandmarkFetchRequest *lmFetchRequest;
    QLandmarkCategoryRemoveRequest *catRemoveRequest;
    QLandmarkRemoveRequest *lmRemoveRequest;
    QLandmarkImportRequest *lmImportRequest;
    QLandmarkExportRequest *lmExportRequest;

    QLandmarkManager *landmarkManager;
    QLandmarkCategoryId categoryId;
    QLandmarkCategory category;
    int previousLastIndex;
};

#endif
