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

#include <qcameraimageprocessingcontrol.h>
#include  "qmediacontrol_p.h"

QT_BEGIN_NAMESPACE

/*!
    \class QCameraImageProcessingControl
    \inmodule QtMultimediaKit
    \ingroup multimedia-serv
    \since 1.1


    \brief The QCameraImageProcessingControl class provides an abstract class
    for controlling image processing parameters, like white balance,
    contrast, saturation, sharpening and denoising.

    The interface name of QCameraImageProcessingControl is \c com.nokia.Qt.QCameraImageProcessingControl/1.0 as
    defined in QCameraImageProcessingControl_iid.



    \sa QMediaService::requestControl(), QCamera
*/

/*!
    \macro QCameraImageProcessingControl_iid

    \c com.nokia.Qt.QCameraImageProcessingControl/1.0

    Defines the interface name of the QCameraImageProcessingControl class.

    \relates QCameraImageProcessingControl
*/

/*!
    Constructs an image processing control object with \a parent.
*/

QCameraImageProcessingControl::QCameraImageProcessingControl(QObject *parent):
    QMediaControl(*new QMediaControlPrivate, parent)
{
}

/*!
    Destruct the image processing control object.
*/

QCameraImageProcessingControl::~QCameraImageProcessingControl()
{
}


/*!
    \fn QCameraImageProcessingControl::whiteBalanceMode() const
    Return the white balance mode being used.
    \since 1.1
*/

/*!
    \fn QCameraImageProcessingControl::setWhiteBalanceMode(QCameraImageProcessing::WhiteBalanceMode mode)
    Set the white balance mode to \a mode
    \since 1.1
*/

/*!
    \fn QCameraImageProcessingControl::isWhiteBalanceModeSupported(QCameraImageProcessing::WhiteBalanceMode mode) const
    Returns true if the white balance \a mode is supported.
    The backend should support at least QCameraImageProcessing::WhiteBalanceAuto mode.
    \since 1.1
*/

/*!
    \fn bool QCameraImageProcessingControl::isProcessingParameterSupported(ProcessingParameter parameter) const

    Returns true if the camera supports adjusting image processing \a parameter.

    Usually the the supported settings is static,
    but some parameter may not be available depending on other
    camera settings, like presets.
    In such case the currently supported parameters should be returned.
    \since 1.1
*/

/*!
    \fn QCameraImageProcessingControl::processingParameter(ProcessingParameter parameter) const
    Returns the image processing \a parameter value.
    \since 1.1
*/

/*!
    \fn QCameraImageProcessingControl::setProcessingParameter(ProcessingParameter parameter, QVariant value)

    Sets the image processing \a parameter \a value.
    Passing the null or invalid QVariant value allows
    backend to choose the suitable parameter value.

    The valid values range depends on the parameter type,
    for contrast, saturation and brightness value should be
    between -100 and 100, the default is 0,

    For sharpening and denoising the range is 0..100,
    0 for sharpening or denoising disabled
    and 100 for maximum sharpening/denoising applied.
    \since 1.1
*/

/*!
  \enum QCameraImageProcessingControl::ProcessingParameter

  \value Contrast
    Image contrast.
  \value Saturation
    Image saturation.
  \value Brightness
    Image brightness.
  \value Sharpening
    Amount of sharpening applied.
  \value Denoising
    Amount of denoising applied.
  \value ColorTemperature
    Color temperature in K. This value is used when the manual white balance mode is selected.
  \value ExtendedParameter
    The base value for platform specific extended parameters.
 */

#include "moc_qcameraimageprocessingcontrol.cpp"
QT_END_NAMESPACE

