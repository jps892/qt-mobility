/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
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

#include <QObject>
#include <qmobilityglobal.h>
#include <qtorganizer.h>
#include <QtTest/QtTest>
#include <QDebug>

QTM_USE_NAMESPACE

const QString managerNameSymbian("symbian");

// We need to be able to pass QOrganizerItemRecurrenceRule as parameter from
// test data functions
Q_DECLARE_METATYPE(QOrganizerItemRecurrenceRule)

/*!
 * For testing recurrence rule implementation of organizer backends via
 * QOrganizerItemManager API. The target is  to implement test cases in a
 * platform independent manner so that this test module could be used with
 * every backends and potentially used as a QtMobility auto test with as little
 * porting work as possible.
 */
class tst_recurringItems : public QObject
{
    Q_OBJECT

private slots:  // Init & cleanup
    void initTestCase();
    void init();
    void cleanup();

private slots:  // Test cases
    void addRecurrenceRule_data();
    void addRecurrenceRule();
    void removeRecurrenceRule_data();
    void removeRecurrenceRule();

private: // util functions
    void addManagers();
    void addItemsWeeklyRecurrence(QString managerName, QString itemType);
    void addItemsDailyRecurrence(QString managerName, QString itemType);
    void addItemsMonthlyRecurrence(QString managerName, QString itemType);
    void addItemsYearlyRecurrence(QString managerName, QString itemType);
    bool verifyRecurrenceRule(
        QOrganizerItemRecurrenceRule expectedRRule,
        QOrganizerItemRecurrenceRule actualRRule);

private:
    QOrganizerItemManager *m_om;
};

void tst_recurringItems::initTestCase()
{
    // Remove all organizer items from available managers
    QStringList managerNames = QOrganizerItemManager::availableManagers();
    managerNames.removeAll("invalid");
    managerNames.removeAll("skeleton");
    foreach (QString managerName, managerNames) {
        m_om = new QOrganizerItemManager(managerName);
        m_om->removeItems(m_om->itemIds(), 0);
    }
}

void tst_recurringItems::init()
{
    QFETCH(QString, managerName);
    m_om = new QOrganizerItemManager(managerName);

    // Remove all organizer items first (Note: ignores possible errors)
    m_om->removeItems(m_om->itemIds(), 0);
}

void tst_recurringItems::cleanup()
{
    // Remove all organizer items first (Note: ignores possible errors)
    m_om->removeItems(m_om->itemIds(), 0);
    delete m_om;
    m_om = 0;
}

void tst_recurringItems::addRecurrenceRule_data()
{
    QTest::addColumn<QString>("managerName");
    QTest::addColumn<QString>("displayLabel");
    QTest::addColumn<QString>("itemType");
    QTest::addColumn<QDateTime>("startTime");
    QTest::addColumn<QOrganizerItemRecurrenceRule>("rrule");

    QStringList managerNames = QOrganizerItemManager::availableManagers();
    managerNames.removeAll("invalid"); // the test cases would not pass on invalid backend
    managerNames.removeAll("skeleton"); // the test cases would not pass on skeleton backend

    foreach (QString managerName, managerNames) {
        addItemsWeeklyRecurrence(managerName, QOrganizerItemType::TypeEvent);
        addItemsDailyRecurrence(managerName, QOrganizerItemType::TypeEvent);
        addItemsYearlyRecurrence(managerName, QOrganizerItemType::TypeEvent);
        addItemsMonthlyRecurrence(managerName, QOrganizerItemType::TypeEvent);
    }
}

void tst_recurringItems::addRecurrenceRule()
{
    // TODO: refactor to support dynamic addition of test cases
    QFETCH(QString, managerName);
    QFETCH(QString, displayLabel);
    QFETCH(QString, itemType);
    QFETCH(QDateTime, startTime);
    QFETCH(QOrganizerItemRecurrenceRule, rrule);

    // Create item
    QOrganizerItem item;
    item.setType(itemType);
    item.setDisplayLabel(displayLabel);
    QOrganizerEventTimeRange timeRange;
    timeRange.setStartDateTime(startTime);
    QVERIFY(item.saveDetail(&timeRange));

    // Add recurrence rules to the item
    QList<QOrganizerItemRecurrenceRule> rrules;
    rrules.append(rrule);
    QOrganizerItemRecurrence recurrence;
    recurrence.setRecurrenceRules(rrules);
    QVERIFY(item.saveDetail(&recurrence));

    // Save
    QVERIFY(m_om->saveItem(&item));

    // Fetch and Verify
    item = m_om->item(item.localId());

    QOrganizerItemRecurrence resultRecurrence = item.detail(QOrganizerItemRecurrence::DefinitionName);

    // The test case uses a single recurrence rule, verify the count
    QCOMPARE(resultRecurrence.recurrenceRules().count(), 1);

    // TODO: The following does not check for extra fields in the retrieved
    // recurrence rule
    // TODO: check also other details
    QVERIFY(verifyRecurrenceRule(rrule, resultRecurrence.recurrenceRules()[0]));
}

/*
 * A helper method for verifying recurrence rule fields one-by-one to make
 * debugging easier
 */
bool tst_recurringItems::verifyRecurrenceRule(
    QOrganizerItemRecurrenceRule expectedRRule,
    QOrganizerItemRecurrenceRule actualRRule)
{
    bool match(true);

    if (expectedRRule.frequency()!= actualRRule.frequency()) {
        match = false;
    }

    if (expectedRRule.count()&&(expectedRRule.count()!= actualRRule.count())) {
        match = false;
    }

    // Verifying end date is not straightforward
    if (expectedRRule.endDate() != actualRRule.endDate()) {
        bool endDateVerified (false);

        // Allow the exceptional case with no end time and no count set (in
        // that case the end date is set to year 2100 by symbian calendar server)
        if (expectedRRule.count() == 0
            && expectedRRule.endDate().isNull()
            && actualRRule.endDate() == QDate(2100, 12, 31)) {
            endDateVerified = true;
        }

        // Symbian calendar server sets both count and end date during save,
        // but count has higher priority. In practice this means that setting
        // end date for an item results in an item with count set.
        if (actualRRule.count() > 0
            && !actualRRule.endDate().isValid()) {
            endDateVerified = true;
        }

        // Symbian calendar server may have updated the end date to match
        // the actual date of the last item instance -> Allow end date that
        // is before the expected end date
        // Note: this rule is not needed in the verification, because end date
        // will never be set on symbian backend. The rule is here for future
        // reference. See the previous verification rule for more details on
        // why end date is not set.
        /*
        if (expectedRRule.endDate().isValid()
            && actualRRule.endDate().isValid()
            && actualRRule.endDate() < expectedRRule.endDate()) {
            endDateVerified = true;
        }
        */

        if (!endDateVerified) {
            qDebug() << "expected end date" << expectedRRule.endDate();
            qDebug() << "actual end date" << actualRRule.endDate();
            match = false;
        }
    }

    if (expectedRRule.interval()&&(expectedRRule.interval()!= actualRRule.interval())) {
        match = false;
    }

    if (expectedRRule.months().count()&&(expectedRRule.months()!= actualRRule.months())) {
        match = false;
    }

    if (expectedRRule.daysOfWeek().count()&&(expectedRRule.daysOfWeek()!= actualRRule.daysOfWeek())) {
        match = false;
    }

    if (expectedRRule.daysOfMonth().count()&&(expectedRRule.daysOfMonth()!= actualRRule.daysOfMonth())) {
        match = false;
    }

    if (expectedRRule.positions().count()&&(expectedRRule.positions()!= actualRRule.positions())) {
        //calentry fetches positions in ascending order
        QList<int>orginalPosition(expectedRRule.positions());
        qSort(orginalPosition);
        if (orginalPosition!= actualRRule.positions()) {
            match = false;
        }
    }

    if (expectedRRule.daysOfYear().count()&&(expectedRRule.daysOfYear()!= actualRRule.daysOfYear())) {
        match = false;
    }

    if (expectedRRule.weeksOfYear().count()&&(expectedRRule.weeksOfYear()!= actualRRule.weeksOfYear())) {
        match = false;
    }

    if (expectedRRule.weekStart()&&(expectedRRule.weekStart()!= actualRRule.weekStart())) {
        match = false;
    }

    return match;
}

void tst_recurringItems::removeRecurrenceRule_data()
{
    // Use the same recurrence rule data as the add test cases use
    addRecurrenceRule_data();
}

void tst_recurringItems::removeRecurrenceRule()
{
    QFETCH(QString, managerName);
    QFETCH(QString, itemType);
    QFETCH(QDateTime, startTime);
    QFETCH(QOrganizerItemRecurrenceRule, rrule);

    // Create an item
    QOrganizerItem item;
    item.setType(itemType);
    QOrganizerEventTimeRange timeRange;
    timeRange.setStartDateTime(startTime);
    QVERIFY(item.saveDetail(&timeRange));

    // Add recurrence rules to the item
    QList<QOrganizerItemRecurrenceRule> rrules;
    rrules.append(rrule);
    QOrganizerItemRecurrence recurrence;
    recurrence.setRecurrenceRules(rrules);
    QVERIFY(item.saveDetail(&recurrence));

    // Save item with recurrence rule.
    QVERIFY(m_om->saveItem(&item));

    // Fetch the saved item
    item = m_om->item(item.localId());

    // Remove a recurrence rule and save the detail to the item.
    QOrganizerItemRecurrence chkRecurrence = item.detail<QOrganizerItemRecurrence>();

    // Set an empty list.
    chkRecurrence.setRecurrenceRules(QList<QOrganizerItemRecurrenceRule>());
    QVERIFY(item.saveDetail(&chkRecurrence));

    //Save the item again without the recurrence rule.
    QVERIFY(m_om->saveItem(&item));

    // Fetch again and Verify
    QOrganizerItem resultItem;
    resultItem = m_om->item(item.localId());

    if (item.detail(QOrganizerItemRecurrence::DefinitionName)!= resultItem.detail(QOrganizerItemRecurrence::DefinitionName)) {
        qDebug() << "Expected: " << item.detail(QOrganizerItemRecurrence::DefinitionName);
        qDebug() << "Actual:   " << resultItem.detail(QOrganizerItemRecurrence::DefinitionName);
        QFAIL("Recurrence items do not match!");
    }
}

/*!
 * Helper method for executing test cases with all the available managers
 */
void tst_recurringItems::addManagers()
{
    QTest::addColumn<QString>("managerName");

    QStringList managerNames = QOrganizerItemManager::availableManagers();
    //managerNames.removeAll("memory");
    managerNames.removeAll("invalid"); // the test cases would not pass on invalid backend
    managerNames.removeAll("skeleton"); // the test cases would not pass on skeleton backend

    foreach(QString mgr, managerNames) {
        QTest::newRow(QString("[%1]").arg(mgr).toLatin1().constData()) << mgr;
    }
}

/*!
 * Helper method for adding the actual recurrence test cases. Tests adding
 * organizer items with weekly recurrences.
 */
void tst_recurringItems::addItemsWeeklyRecurrence(QString managerName, QString itemType)
{
    QOrganizerItemRecurrenceRule rrule;
    QList<Qt::DayOfWeek> daysOfWeek;

    rrule.setFrequency(QOrganizerItemRecurrenceRule::Weekly);
    QTest::newRow(QString("[%1] weekly forever").arg(managerName).toLatin1().constData())
        << managerName
        << QString("weekly 0")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    rrule = QOrganizerItemRecurrenceRule(); // reset
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Weekly);
    rrule.setCount(10);
    if (!managerName.contains(managerNameSymbian)) {
        // TODO: Symbian calendar server does not allow saving weekly entries
        // without "by day" data. To be studied if this could be implemented
        // on symbian with some work-around. But for now, let's just disable
        // the test case for symbian backend.
        QTest::newRow(QString("[%1] weekly for 10 occurrences").arg(managerName).toLatin1().constData())
            << managerName
            << QString("weekly 1")
            << itemType
            << QDateTime::currentDateTime().addSecs(3600)
            << rrule;
    }

    rrule = QOrganizerItemRecurrenceRule(); // reset
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Weekly);
    rrule.setCount(3);
    daysOfWeek.clear(); // reset
    daysOfWeek.append(Qt::Tuesday);
    rrule.setDaysOfWeek(daysOfWeek);
    QTest::newRow(QString("[%1] weekly on Tuesday for 3 weeks").arg(managerName).toLatin1().constData())
        << managerName
        << QString("weekly 2")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    rrule = QOrganizerItemRecurrenceRule(); // reset
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Weekly);
    rrule.setCount(5);
    daysOfWeek.clear(); // reset
    daysOfWeek.append(Qt::Tuesday);
    daysOfWeek.append(Qt::Thursday);
    rrule.setDaysOfWeek(daysOfWeek);
    rrule.setWeekStart(Qt::Sunday);
    QTest::newRow(QString("[%1] weekly on Tuesday and Thursday for 5 weeks").arg(managerName).toLatin1().constData())
        << managerName
        << QString("weekly 3")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    rrule = QOrganizerItemRecurrenceRule(); // reset
    rrule.setInterval(2);
    daysOfWeek.clear(); // reset
    daysOfWeek.append(Qt::Monday);
    daysOfWeek.append(Qt::Wednesday);
    daysOfWeek.append(Qt::Friday);
    rrule.setDaysOfWeek(daysOfWeek);
    rrule.setEndDate(QDate(QDate::currentDate().year() , 12, 24));
    QTest::newRow(QString("[%1] Every other week on monday,wednesday and friday until 24th dec 2010").arg(managerName).toLatin1().constData())
        << managerName
        << QString("weekly 4")
        << itemType
        << QDateTime(QDate(QDate::currentDate().year() , 9, 1))
        << rrule;

    rrule = QOrganizerItemRecurrenceRule(); // reset
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Weekly);
    daysOfWeek.clear(); // reset
    daysOfWeek.append(Qt::Tuesday);
    daysOfWeek.append(Qt::Thursday);
    rrule.setDaysOfWeek(daysOfWeek);
    rrule.setEndDate(QDate(QDate::currentDate().year() + 1, 1, 24));
    QTest::newRow(QString("[%1] weekly on Tuesday and Thursday until 12/24 of next year").arg(managerName).toLatin1().constData())
        << managerName
        << QString("weekly 5")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;
}

/*!
 * Helper method for adding the actual recurrence test cases. Tests adding
 * organizer items with monthly recurrences.
 */
void tst_recurringItems::addItemsMonthlyRecurrence(QString managerName, QString itemType)
{
    // On 15th day for 3 months
    QOrganizerItemRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Monthly);
    rrule.setCount(3);
    QList<int> daysOfMonth;
    daysOfMonth.append(15);
    rrule.setDaysOfMonth(daysOfMonth);
    QTest::newRow(QString("[%1] monthly on 15th day for 3 months").arg(managerName).toLatin1().constData())
        << managerName
        << QString("monthly 1")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    // Symbian recurrence rule does not support positions in this context so
    // the test case is disabled
    if (!managerName.contains(managerNameSymbian)) {
        // Every other Month on first and last Sunday for 10 occurances
        QOrganizerItemRecurrenceRule rrule1;
        QList<Qt::DayOfWeek> daysOfWeek;
        rrule1.setFrequency(QOrganizerItemRecurrenceRule::Monthly);
        rrule1.setCount(10);
        rrule1.setInterval(2);
        daysOfWeek.append(Qt::Sunday);
        rrule1.setDaysOfWeek(daysOfWeek);
        QList<int> positions;
        positions.append(1);
        positions.append(-1);
        rrule1.setPositions(positions);
        QTest::newRow(QString("[%1] Every other Month on first and last Sunday for 10 occurances").arg(managerName).toLatin1().constData())
            << managerName
            << QString("monthly 2")
            << itemType
            << QDateTime::currentDateTime().addSecs(3600)
            << rrule1;
    }

    // Symbian recurrence rule does not support days-of-week for monthly
    // recurring items so the test case is disabled
    if (!managerName.contains(managerNameSymbian)) {
        QList<Qt::DayOfWeek> daysOfWeek;
        // Every other month every tuesday
        QOrganizerItemRecurrenceRule rrule2;
        rrule2.setFrequency(QOrganizerItemRecurrenceRule::Monthly);
        rrule2.setInterval(2);
        daysOfWeek.clear();
        daysOfWeek.append(Qt::Tuesday);
        rrule2.setDaysOfWeek(daysOfWeek);
        QTest::newRow(QString("[%1] Every other month every tuesday").arg(managerName).toLatin1().constData())
            << managerName
            << QString("monthly 3")
            << itemType
            << QDateTime::currentDateTime().addSecs(3600)
            << rrule2;
    }

    // Symbian recurrence rule does not support positions in this context so
    // the test case is disabled
    if (!managerName.contains(managerNameSymbian)) {
        rrule.setDaysOfMonth(QList<int>()); // clear days of month
        QList<Qt::DayOfWeek> daysOfWeek;
        daysOfWeek.append(Qt::Sunday);
        rrule.setDaysOfWeek(daysOfWeek);
        rrule.setCount(3);
        QList<int> positions;
        positions.clear();
        positions.append(1);
        rrule.setPositions(positions);
        QTest::newRow(QString("[%1] the 1st Sunday of the month for 3 months").arg(managerName).toLatin1().constData())
            << managerName
            << QString("monthly 4")
            << itemType
            << QDateTime::currentDateTime().addSecs(3600)
            << rrule;
    }
}

/*!
 * Helper method for adding the actual recurrence test cases. Tests adding
 * organizer items with daily recurrences.
 */
void tst_recurringItems::addItemsDailyRecurrence(QString managerName, QString itemType)
{
    /* TODO: enable the test cases and implement on symbian backend */
    QOrganizerItemRecurrenceRule rrule;
    rrule.setCount(3);
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Daily);

    QTest::newRow(QString("[%1] daily, Count=3").arg(managerName).toLatin1().constData())
        << managerName
        << QString("daily 1")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    //occurance with interval
    rrule.setCount(5);
    rrule.setInterval(2);

    QTest::newRow(QString("[%1] daily, Count=5 Interval = 2").arg(managerName).toLatin1().constData())
        << managerName
        << QString("daily 2")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    rrule.setCount(0);
    rrule.setInterval(2);
    rrule.setEndDate(QDate(QDate::currentDate().year(), QDate::currentDate().month(), QDate::currentDate().day()+4));

    QTest::newRow(QString("[%1] daily, End Date Interval = 2").arg(managerName).toLatin1().constData())
        << managerName
        << QString("daily 3")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    //Every day in January for 3 years
    /*rrule.setCount(0);
    rrule.setInterval(1);
    rrule.setEndDate(QDate(QDate::currentDate().year()+3,1,31));
    QList<QOrganizerItemRecurrenceRule::Month> oneMonth;
    oneMonth.append(QOrganizerItemRecurrenceRule::January);
    rrule.setMonths(oneMonth);
    QTest::newRow(QString("[%1] Every day in January for 3 years").arg(managerName).toLatin1().constData())
        << managerName
        << QString("daily 4")
        << itemType
        << QDateTime(QDate(QDate::currentDate().year(),1,1)).addSecs(3600)
        << rrule;*/

    // TODO: should this fail? (daily recurring event that has "days of week" set..?)
    /*QList<Qt::DayOfWeek> daysOfWeek;
    daysOfWeek.append(Qt::Monday);
    rrule.setDaysOfWeek(daysOfWeek);

    QTest::newRow(QString("[%1] daily, days of week=Monday;Sunday, Count=3").arg(managerName).toLatin1().constData())
        << managerName
        << QString("daily 5")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;
        */
}

void tst_recurringItems::addItemsYearlyRecurrence(QString managerName, QString itemType)
{
    // Every year for 3 occurances
    QOrganizerItemRecurrenceRule rrule;
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
    rrule.setCount(3);
    QTest::newRow(QString("[%1] yearly").arg(managerName).toLatin1().constData())
        << managerName
        << QString("yearly 1")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    // Yearly on first Monday of January for 2 occurrences
    rrule = QOrganizerItemRecurrenceRule();
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
    rrule.setCount(2);
    QList<QOrganizerItemRecurrenceRule::Month> months;
    months.append(QOrganizerItemRecurrenceRule::January);
    rrule.setMonths(months);
    QList<Qt::DayOfWeek> daysOfWeek;
    daysOfWeek.append(Qt::Monday);
    rrule.setDaysOfWeek(daysOfWeek);
    QList <int> positions;
    positions.append(1);
    rrule.setPositions(positions);
    QTest::newRow(QString("[%1] yearly on first Monday of January").arg(managerName).toLatin1().constData())
        << managerName
        << QString("yearly 2")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    // yearly on every Monday of January
    rrule = QOrganizerItemRecurrenceRule();
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
    months = QList<QOrganizerItemRecurrenceRule::Month>();
    months.append(QOrganizerItemRecurrenceRule::January);
    rrule.setMonths(months);
    daysOfWeek = QList<Qt::DayOfWeek>();
    daysOfWeek.append(Qt::Monday);
    rrule.setDaysOfWeek(daysOfWeek);
    QTest::newRow(QString("[%1] yearly on Mondays of January").arg(managerName).toLatin1().constData())
        << managerName
        << QString("yearly 3")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    // Yearly on 15th day of January
    // Note: Symbian calendar server does not support yearly events with days-of-month
    if (!managerName.contains(managerNameSymbian)) {
        rrule = QOrganizerItemRecurrenceRule();
        rrule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
        rrule.setCount(2);
        QList<QOrganizerItemRecurrenceRule::Month> months;
        months.append(QOrganizerItemRecurrenceRule::January);
        rrule.setMonths(months);
        QList<int> daysOfMonth;
        daysOfMonth.append(15);
        rrule.setDaysOfMonth(daysOfMonth);
        QTest::newRow(QString("[%1] yearly on 15th day of January").arg(managerName).toLatin1().constData())
            << managerName
            << QString("yearly 4")
            << itemType
            << QDateTime::currentDateTime().addSecs(3600)
            << rrule;
    }

/*  TODO: Enable the following test cases and implement them on symbian backend

    //Every year in january and march for 4 occurances
    // TODO: Symbian calendar server does not allow setting a yearly recurring
    // event with "months" set but without setting "day of month"
    rrule = QOrganizerItemRecurrenceRule();
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
    rrule.setCount(4);
    months = QList<QOrganizerItemRecurrenceRule::Month>();
    months.append(QOrganizerItemRecurrenceRule::January);
    months.append(QOrganizerItemRecurrenceRule::March);
    rrule.setMonths(months);
    QTest::newRow(QString("[%1] yearly on January and March").arg(managerName).toLatin1().constData())
        << managerName
        << QString("yearly 5")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    //Every other year in january,feburary and march for 4 occurances
    rrule = QOrganizerItemRecurrenceRule();
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
    rrule.setCount(4);
    rrule.setInterval(2);
    months.clear();
    months.append(QOrganizerItemRecurrenceRule::January);
    months.append(QOrganizerItemRecurrenceRule::February);
    months.append(QOrganizerItemRecurrenceRule::March);
    rrule.setMonths(months);
    QTest::newRow(QString("[%1] every other year on January Feburary and March").arg(managerName).toLatin1().constData())
        << managerName
        << QString("yearly 6")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    //Every 3rd year on 1st,100th and 200th day for 10 occurances
    rrule = QOrganizerItemRecurrenceRule();
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
    rrule.setCount(10);
    rrule.setInterval(3);
    QList<int> daysOfYear;
    daysOfYear.append(1);
    daysOfYear.append(100);
    daysOfYear.append(200);
    rrule.setDaysOfYear(daysOfYear);
    QTest::newRow(QString("[%1] every 3rd year on 1st,100th and 200th day for 10 occurances").arg(managerName).toLatin1().constData())
        << managerName
        << QString("yearly 7")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    //Every 20th monday of the year,forever
    rrule = QOrganizerItemRecurrenceRule();
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
    rrule.setCount(0);
    rrule.setInterval(1);
    daysOfWeek = QList<Qt::DayOfWeek>();
    daysOfWeek.append(Qt::Monday);
    rrule.setDaysOfWeek(daysOfWeek);
    QList<int> pos;
    pos.append(20);
    rrule.setPositions(pos);
    QTest::newRow(QString("[%1] Every 20th monday of the year,forever").arg(managerName).toLatin1().constData())
        << managerName
        << QString("yearly 8")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    //Monday of week number 20 forever
    rrule = QOrganizerItemRecurrenceRule();
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
    QList<int> weekNumber;
    weekNumber.append(20);
    rrule.setWeeksOfYear(weekNumber);
    rrule.setDaysOfWeek(daysOfWeek);
    QTest::newRow(QString("[%1] Monday of week number 20 forever").arg(managerName).toLatin1().constData())
        << managerName
        << QString("yearly 9")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    //Every Monday in march forever
    rrule = QOrganizerItemRecurrenceRule();
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
    QList<QOrganizerItemRecurrenceRule::Month> oneMonth;
    oneMonth.append(QOrganizerItemRecurrenceRule::March);
    rrule.setMonths(oneMonth);
    rrule.setDaysOfWeek(daysOfWeek);
    QTest::newRow(QString("[%1] Every Monday in march forever").arg(managerName).toLatin1().constData())
        << managerName
        << QString("yearly 10")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    rrule = QOrganizerItemRecurrenceRule();
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
    QList<int> daysOfMonth;
    daysOfMonth.append(1);
    rrule.setDaysOfMonth(daysOfMonth);
    rrule.setCount(4);
    //Occurs every 1st day of the month for 4 occurances
    QTest::newRow(QString("[%1] Days of Month=1, Count=4").arg(managerName).toLatin1().constData())
        << managerName
        << QString("yearly 11")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;

    rrule = QOrganizerItemRecurrenceRule();
    rrule.setFrequency(QOrganizerItemRecurrenceRule::Yearly);
    daysOfYear.clear();
    daysOfYear.append(32);
    rrule.setDaysOfYear(daysOfYear);
    rrule.setCount(1);
    QTest::newRow(QString("[%1] Days of Year=32, Count=1").arg(managerName).toLatin1().constData())
        << managerName
        << QString("yearly 12")
        << itemType
        << QDateTime::currentDateTime().addSecs(3600)
        << rrule;
*/
}

QTEST_MAIN(tst_recurringItems);
#include "tst_recurringItems.moc"