/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the tools applications of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QMLTCTYPERESOLVER_H
#define QMLTCTYPERESOLVER_H

#include <QtQml/private/qqmlirbuilder_p.h>
#include <private/qqmljstyperesolver_p.h>
#include <private/qqmljsimporter_p.h>
#include <private/qqmljslogger_p.h>

QT_BEGIN_NAMESPACE

class QmltcTypeResolver : public QQmlJSTypeResolver
{
public:
    QmltcTypeResolver(QQmlJSImporter *importer, const QmlIR::Document *document,
                      TypeStorage storage, Semantics semantics, QQmlJSLogger *logger)
        : QQmlJSTypeResolver(importer, document, storage, semantics, logger)
    {
    }
};

QT_END_NAMESPACE

#endif // QMLTCTYPERESOLVER_H