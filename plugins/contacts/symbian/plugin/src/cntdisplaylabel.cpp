/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the QtCore module of the Qt Toolkit.
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

#include "cntdisplaylabel.h"
#include <qtcontacts.h>

/*! 
 * Constructor
 */

CntDisplayLabel::CntDisplayLabel()
{
    m_settings = new CntCenrep(*this);
    m_nameOrder = m_settings->getValue();
    setDisplayLabelDetails();
}

/*! 
 * Destructor
 */
CntDisplayLabel::~CntDisplayLabel()
{
    delete m_settings;
}

/*! 
 * Sets the fields that should be used for displaylabel
 * 
 * The order is defined in the QList of preferred options
 * Whereas the QPair includes the fields that should be used for 
 * the specific detail
 */
void CntDisplayLabel::setDisplayLabelDetails()
{
    //This will be variated in the future
    //adds the contact's details definition names + fields to be used for display name and filtering display label

    //Display label details
    //Contact
    //Preferred details
    m_contactDisplayLabelDetails.clear();
    m_groupDisplayLabelDetails.clear();
    QList<QPair<QLatin1String, QLatin1String> > contactPrefferedDisplayLabelDetails;
    QList<QPair<QLatin1String, QLatin1String> > contactOptionalDisplayLabelDetails;
    QLatin1String firstLatin(QContactName::FieldFirstName);
    QLatin1String secondLatin(QContactName::FieldLastName);
    
    if (m_nameOrder == CntOrderLastFirst || m_nameOrder == CntOrderLastCommaFirst) {
        firstLatin = QLatin1String(QContactName::FieldLastName);
        secondLatin = QLatin1String(QContactName::FieldFirstName);
    }
    
    contactPrefferedDisplayLabelDetails.append(qMakePair(QLatin1String(QContactName::DefinitionName), firstLatin));
    contactPrefferedDisplayLabelDetails.append(qMakePair(QLatin1String(QContactName::DefinitionName), secondLatin));
    m_contactDisplayLabelDetails.append(contactPrefferedDisplayLabelDetails);
    contactOptionalDisplayLabelDetails.append(qMakePair(QLatin1String(QContactName::DefinitionName), QLatin1String(QContactName::FieldCustomLabel)));
    m_contactDisplayLabelDetails.append(contactOptionalDisplayLabelDetails);
    
    //Group
    QList<QPair<QLatin1String, QLatin1String> > preferredGroupDisplayLabelDetails;
    preferredGroupDisplayLabelDetails.append(qMakePair(QLatin1String(QContactName::DefinitionName), QLatin1String(QContactName::FieldCustomLabel)));
    m_groupDisplayLabelDetails.append(preferredGroupDisplayLabelDetails);
}

/*! 
 * Returns a synthesised display label of a contact
 * \a contact to read the data from .
 * \a error On return, contains the possible error.
 * \return synthesised display label 
 */
QString CntDisplayLabel::synthesizedDisplayLabel(const QContact& contact, QContactManager::Error* error)
{
    QString displayLabel;
    *error = QContactManager::NoError;
    
    int value = m_settings->getValue();
    if (value != -1 && value != m_nameOrder) {
        m_nameOrder = value;
        setDisplayLabelDetails();
    }
    //contact
    if(contact.type() == QContactType::TypeContact) {
        displayLabel = generateDisplayLabel(contact, m_contactDisplayLabelDetails);    
    }
    
    //group
    else if (contact.type() == QContactType::TypeGroup) {
        displayLabel = generateDisplayLabel(contact, m_groupDisplayLabelDetails);
    } 
    
    //invalid type
    else {
        *error = QContactManager::InvalidContactTypeError;
    }
    
    return displayLabel;
}

/*! 
 * Generate the display label
 * \a contact to read the data from .
 * \a detailList contains the details to be read from the contact
 * \return synthesised display label 
 */
QString CntDisplayLabel::generateDisplayLabel( const QContact &contact, const QList<QList<QPair<QLatin1String, QLatin1String> > > detailList) const
{
    // Default to empty display label. It is up to the client to create a
    // localised presentation of a contact without a name.
    QString displayLabel("");
    
    //loop through the details and create display label
    for(int i = 0; i < detailList.count() && displayLabel.isEmpty(); i++ )
    {
        QList<QPair<QLatin1String, QLatin1String> > detailPairList = detailList.at(i);
        QContactDetail contactDetail;
        
        for(int j = 0; j < detailPairList.count(); j++)
        {
            contactDetail = contact.detail(detailPairList.at(j).first);
                    
            if(displayLabel.isEmpty()){ //read the value and set it as display label
                displayLabel =  contactDetail.value(detailPairList.at(j).second);
            }
            else{ //read the value and append it to the display label
                QString label = contactDetail.value(detailPairList.at(j).second);
                
                if(!label.isEmpty())
                {
                    // Inlcude a comma if needed in the display label
                    if (m_nameOrder == CntOrderLastCommaFirst)
                        displayLabel.append(comma());

                    displayLabel.append(delimiter());                        
                    displayLabel.append(label);
                }  
            }
        }
    }

    return displayLabel;
}

/*! 
 * delemiter to be used in display label
 * \return delemiter 
 */
QString CntDisplayLabel::delimiter() const
{
    //this will be variated in the future.
    return " ";
}

/*!
 * Returns the details to be used for contact filtering
 * first value: detail definition
 * second value: field definition
 */
QList<QPair<QLatin1String, QLatin1String> > CntDisplayLabel::contactFilterDetails() const
{
    return m_contactDisplayLabelDetails.at(0);
}

/*!
 * Returns the details to be used for group filtering
 * first value: detail definition
 * second value: field definition
 */
QList<QPair<QLatin1String, QLatin1String> > CntDisplayLabel::groupFilterDetails() const
{
    return m_groupDisplayLabelDetails.at(0);
}

/*! 
 * Comma to be used in display label
 * \return comma
 */
QString CntDisplayLabel::comma() const
{
    return ",";
}

void CntDisplayLabel::updateNameOrdering()
{
    emit displayLabelChanged();
}

CntCenrep::CntCenrep(MDisplayLabel& aDisplayLabel) :
    CActive(EPriorityStandard),
    iCenrep(NULL),
    iDisplayLabel(aDisplayLabel),
    iKey(KCntNameOrdering)
{   
    TRAPD(error, iCenrep = CRepository::NewL(KCRCntSettings));
    
    if ( error == KErrNone ) {
        CActiveScheduler::Add(this);
        
        // initial subscription and process current property value
        iCenrep->NotifyRequest(iKey, iStatus );
        SetActive();	
    } else {
        TRAPD(error, iCenrep = CRepository::NewL(KCRUidPhonebook));
        if ( error == KErrNone ) {
            CActiveScheduler::Add(this);
            
            iKey = KPhonebookNameOrdering;
            iCenrep->NotifyRequest(iKey, iStatus );
            SetActive();
        }	
    }
    
    iValue = getValue();
}

CntCenrep::~CntCenrep()
{
    Cancel();
    delete iCenrep;
}

int CntCenrep::getValue()
{    
    TInt value;
    if (iCenrep && iCenrep->Get(iKey, value) == KErrNone) {
        return value;
    } else {
        return -1;
    }
}

void CntCenrep::DoCancel()
{
    if (iCenrep)
        iCenrep->NotifyCancel(iKey);
}

TInt CntCenrep::RunError(TInt aError)
{
    return aError;
}

void CntCenrep::RunL()
{
    // resubscribe before processing new value to prevent missing updates
    iCenrep->NotifyRequest(iKey, iStatus );
    SetActive();
    
    int value = getValue();
    if (value != -1 && value != iValue) {
        iValue = value;
        iDisplayLabel.updateNameOrdering();
    }
}
