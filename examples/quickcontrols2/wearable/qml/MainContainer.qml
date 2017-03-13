/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.7
import QtQuick.Layouts 1.3
import QtQuick.Controls 2.0 as QQC2
import QtQml.StateMachine 1.0 as DeclSM
import "WatchFace"
import "Fitness"
import "Navigation"
import "Style"

Item {
    Item {
        id: homeCntrl

        z: 2

        anchors.top: parent.top
        anchors.left: parent.left
        anchors.right: parent.right

        height: 40

        states: State {
            name: "slideOut"

            AnchorChanges {
                target: homeButton
                anchors.bottom: homeCntrl.bottom
            }
        }

        transitions: Transition {
            AnchorAnimation {
                duration: 250
            }
        }

        state: (stackView.depth > 1)  ? "slideOut" : ""

        Item {
            id: homeButton

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.top

            height: parent.height

            Rectangle {
                height: parent.height * 4
                width: height
                radius: width / 2

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.top
                anchors.verticalCenterOffset: -height / 4

                color: UIStyle.colorQtGray2
            }

            Image {
                height: parent.height
                width: height

                anchors.horizontalCenter: parent.horizontalCenter

                source: "../images/home.png"
            }
        }

        MouseArea {
            id: homeCntrlMArea
            anchors.fill: parent
            onClicked: {
                homeCntrl.homeKeyPressed()
            }
        }

        function homeKeyPressed() {
            stackView.pop()
        }
    }

    QQC2.StackView {
        id: stackView

        anchors.top: homeCntrl.bottom
        anchors.bottom: backCntrl.top

        width: parent.width

        focus: true

        initialItem: LauncherMain {
        }
    }

    Item {
        id: backCntrl

        z: 2

        anchors.bottom: parent.bottom
        anchors.left: parent.left
        anchors.right: parent.right

        height: 40

        states: State {
            name: "slideOut"

            AnchorChanges {
                target: backButton
                anchors.top: backCntrl.top
            }
        }

        transitions: Transition {
            AnchorAnimation {
                duration: 250
            }
        }

        state: (stackView.depth > 1) ? "slideOut" : ""

        Item {
            id: backButton

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.top: parent.bottom

            height: parent.height

            Rectangle {
                height: parent.height * 4
                width: height
                radius: width / 2

                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.bottom
                anchors.verticalCenterOffset: height / 4

                color: UIStyle.colorQtGray2
            }

            Image {
                height: parent.height
                width: height

                anchors.horizontalCenter: parent.horizontalCenter

                source: "../images/back.png"
            }
        }

        MouseArea {
            id: backCntrlMArea
            anchors.fill: parent
            onClicked: {
                backCntrl.backKeyPressed()
            }
        }

        function backKeyPressed() {
            stackView.pop()
        }
    }
}
