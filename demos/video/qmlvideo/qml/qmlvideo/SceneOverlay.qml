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

import QtQuick 1.0

Scene {
    id: root
    property int margin: 20
    property string contentType

    Content {
        id: content
        anchors.centerIn: parent
        width: parent.contentWidth
        contentType: root.contentType
        source: parent.source1
        volume: parent.volume
        onVideoFramePainted: root.videoFramePainted()
    }

    Rectangle {
        id: overlay
        y: 0.5 * parent.height
        width: content.width
        height: content.height
        color: "yellow"
        opacity: 0.5

        SequentialAnimation on x {
            id: xAnimation
            loops: Animation.Infinite
            property int from: margin
            property int to: 100
            property int duration: 1500
            running: false
            PropertyAnimation {
                from: xAnimation.from
                to: xAnimation.to
                duration: xAnimation.duration
                easing.type: Easing.InOutCubic
            }
            PropertyAnimation {
                from: xAnimation.to
                to: xAnimation.from
                duration: xAnimation.duration
                easing.type: Easing.InOutCubic
            }
        }

        SequentialAnimation on y {
            id: yAnimation
            loops: Animation.Infinite
            property int from: margin
            property int to: 180
            property int duration: 1500
            running: false
            PropertyAnimation {
                from: yAnimation.from
                to: yAnimation.to
                duration: yAnimation.duration
                easing.type: Easing.InOutCubic
            }
            PropertyAnimation {
                from: yAnimation.to
                to: yAnimation.from
                duration: yAnimation.duration
                easing.type: Easing.InOutCubic
            }
        }
    }

    onWidthChanged: {
        xAnimation.to = root.width - content.width - margin
        xAnimation.start()
    }

    onHeightChanged: {
        //yAnimation.to = root.height - content.height - margin
        yAnimation.start()
    }

    Component.onCompleted: root.content = content
}
