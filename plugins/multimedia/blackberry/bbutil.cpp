/****************************************************************************
**
** Copyright (C) 2012 Research In Motion
**
** Contact: Research In Motion <blackberry-qt@qnx.com>
** Contact: Klarälvdalens Datakonsult AB <info@kdab.com>
**
** This file is part of the Qt Mobility Components.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/
#include "bbutil.h"

#include <QtCore/qstring.h>

QT_BEGIN_NAMESPACE

struct MmError {
    int errorCode;
    const char *name;
};

#define MM_ERROR_ENTRY(error) { error, #error }
static const MmError mmErrors[] = {
    MM_ERROR_ENTRY(MMR_ERROR_NONE),
    MM_ERROR_ENTRY(MMR_ERROR_UNKNOWN ),
    MM_ERROR_ENTRY(MMR_ERROR_INVALID_PARAMETER ),
    MM_ERROR_ENTRY(MMR_ERROR_INVALID_STATE),
    MM_ERROR_ENTRY(MMR_ERROR_UNSUPPORTED_VALUE),
    MM_ERROR_ENTRY(MMR_ERROR_UNSUPPORTED_MEDIA_TYPE),
    MM_ERROR_ENTRY(MMR_ERROR_MEDIA_PROTECTED),
    MM_ERROR_ENTRY(MMR_ERROR_UNSUPPORTED_OPERATION),
    MM_ERROR_ENTRY(MMR_ERROR_READ),
    MM_ERROR_ENTRY(MMR_ERROR_WRITE),
    MM_ERROR_ENTRY(MMR_ERROR_MEDIA_UNAVAILABLE),
    MM_ERROR_ENTRY(MMR_ERROR_MEDIA_CORRUPTED),
    MM_ERROR_ENTRY(MMR_ERROR_OUTPUT_UNAVAILABLE),
    MM_ERROR_ENTRY(MMR_ERROR_NO_MEMORY),
    MM_ERROR_ENTRY(MMR_ERROR_RESOURCE_UNAVAILABLE),
    MM_ERROR_ENTRY(MMR_ERROR_MEDIA_DRM_NO_RIGHTS),
    MM_ERROR_ENTRY(MMR_ERROR_DRM_CORRUPTED_DATA_STORE),
    MM_ERROR_ENTRY(MMR_ERROR_DRM_OUTPUT_PROTECTION),
    MM_ERROR_ENTRY(MMR_ERROR_DRM_OPL_HDMI),
    MM_ERROR_ENTRY(MMR_ERROR_DRM_OPL_DISPLAYPORT),
    MM_ERROR_ENTRY(MMR_ERROR_DRM_OPL_DVI),
    MM_ERROR_ENTRY(MMR_ERROR_DRM_OPL_ANALOG_VIDEO),
    MM_ERROR_ENTRY(MMR_ERROR_DRM_OPL_ANALOG_AUDIO),
    MM_ERROR_ENTRY(MMR_ERROR_DRM_OPL_TOSLINK),
    MM_ERROR_ENTRY(MMR_ERROR_DRM_OPL_SPDIF),
    MM_ERROR_ENTRY(MMR_ERROR_DRM_OPL_BLUETOOTH),
    MM_ERROR_ENTRY(MMR_ERROR_DRM_OPL_WIRELESSHD),
};
static const int numMmErrors = sizeof(mmErrors) / sizeof(MmError);

QString mmErrorMessage(const QString &msg, mmr_context_t *context, int *errorCode)
{
    const mmr_error_info_t * const mmError = mmr_error_info(context);

    if (errorCode)
        *errorCode = mmError->error_code;

    if (mmError->error_code >= 0 && mmError->error_code < numMmErrors) {
        return QString("%1: %2 (code %3)").arg(msg).arg(mmErrors[mmError->error_code].name)
                                          .arg(mmError->error_code);
    } else {
        return QString("%1: Unknown error code %2").arg(msg).arg(mmError->error_code);
    }
}

QT_END_NAMESPACE