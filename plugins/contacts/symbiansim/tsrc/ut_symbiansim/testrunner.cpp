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

#include "testrunner.h"
#include <QtTest/QtTest>
#include <QDir>
#include <stdio.h>

const char testFunctionElement[] = "TestFunction";
const char incidentElement[] = "Incident";
const char descriptionElement[] = "Description";
const char nameAttr[] = "name";
const char typeAttr[] = "type";
const char fileAttr[] = "file";
const char lineAttr[] = "line";
const char attrValueFail[] = "fail";


TestRunner::TestRunner(const QString& name)
: mTestCount(0),
  mParsingIncidentElement(false),
  mParsingDescriptionElement(false),
  mCurrentTestFailed(false),
  mCurrentTestFailureLine(0)
{
    mTestRunParams.append(name);
    mTestRunParams.append("-xml");
    mTestRunParams.append("-o");
    mHomeDir = QDir::homePath();
    mTestRunParams.append(QString()); // Initial result file name

    if (!mHomeDir.endsWith(QString::fromAscii("/")))
        mHomeDir += QString::fromAscii("/");
}

TestRunner::~TestRunner()
{
}

int TestRunner::runTests(QObject& testObject)
{
    QString className(testObject.metaObject()->className());
    printf("Running tests for %s ... ", className.toUtf8().data());
    QString resultFileName = mHomeDir + className + ".xml";
    mTestRunParams.replace(mTestRunParams.count()-1,resultFileName);
    int errorsBefore = mErrors.count();
    int error = QTest::qExec(&testObject, mTestRunParams);
    parse(resultFileName);
    printf("Failures: %d\n",mErrors.count()-errorsBefore);
    fflush(stdout);
    return error;
}

void TestRunner::printResults()
{
    printf("\nTests executed: %d\n",mTestCount);
    if (mErrors.count() > 0) {
        printf("Failures (%d):\n", mErrors.count());
        foreach(const QString& error, mErrors) {
            printf("\n%s", error.toUtf8().data());
        }
        printf("\n");
    } else {
        printf("All passed.\n\n");
    }
    fflush(stdout);
}

void TestRunner::parse(const QString& fileName)
{
    QFile file(fileName);
    QXmlInputSource inputSource(&file);
    QXmlSimpleReader reader;
    reader.setContentHandler(this);
    reader.parse(inputSource);
}

bool TestRunner::startElement(
    const QString& /*namespaceURI*/, 
    const QString& /*localName*/, 
    const QString& qName, 
    const QXmlAttributes& atts)
{
    if (qName == QString::fromAscii(testFunctionElement)) {
        mTestCount++;
        mCurrentTestName = atts.value(QString::fromAscii(nameAttr));
        return true;
    }
    if (qName == QString::fromAscii(incidentElement)) {
        mParsingIncidentElement = true;
        if (atts.value(QString::fromAscii(typeAttr)) == QString::fromAscii(attrValueFail)) {
            mCurrentTestFailed = true;
            mCurrentTestFile = atts.value(QString::fromAscii(fileAttr));
            mCurrentTestFailureLine = atts.value(QString::fromAscii(lineAttr)).toInt();
        }
        return true;
    }
    mParsingDescriptionElement =
        (qName == QString::fromAscii(descriptionElement));
    return true;
}

bool TestRunner::endElement(
    const QString& /*namespaceURI*/,
    const QString& /*localName*/,
    const QString& qName)
{
    if (qName == QString::fromAscii(incidentElement)) {
        mParsingIncidentElement = false;
        mCurrentTestFailed = false;
        return true;
    }
    if (qName == QString::fromAscii(descriptionElement)) {
        mParsingDescriptionElement = false;
    }    
    return true;
}

bool TestRunner::characters(const QString& ch)
{
    if (mParsingIncidentElement && 
        mParsingDescriptionElement &&
        mCurrentTestFailed) {
        QByteArray testResult = mCurrentTestName.toAscii() + " failed:\n";
        testResult += "File: ";
        testResult += mCurrentTestFile.toAscii();
        testResult += '\n';
        testResult += "Line: ";
        testResult += QByteArray::number(mCurrentTestFailureLine);
        testResult += '\n';
        testResult += "Reason: ";
        testResult += ch.toAscii();
        testResult += '\n';
        mErrors.append(QString::fromAscii(testResult.data()));
    }
    return true;
}

