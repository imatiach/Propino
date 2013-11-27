/****************************************************************************
**
** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of Qt Creator.
**
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
****************************************************************************/

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of Qt Designer.  This header
// file may change from version to version without notice, or even be removed.
//
// We mean it.
//

#ifndef SHAREDENUMS_H
#define SHAREDENUMS_H

#include "shared_global_p.h"

QT_BEGIN_NAMESPACE

namespace qdesigner_internal {

    // Validation mode of text property line edits
    enum TextPropertyValidationMode {
        // Allow for multiline editing using literal "\n".
        ValidationMultiLine,
        // Allow for HTML rich text including multiline editing using literal "\n".
        ValidationRichText,
        // Validate a stylesheet
        ValidationStyleSheet,
        // Single line mode, suppresses newlines
        ValidationSingleLine,
        // Allow only for identifier characters
        ValidationObjectName,
        // Allow only for identifier characters and colons
        ValidationObjectNameScope,
        // URL
        ValidationURL
        };

    // Container types
    enum ContainerType {
        // A container with pages, at least one of which one must always be present (for example, QTabWidget)
        PageContainer,
        // Mdi type container. All pages may be deleted, no concept of page order
        MdiContainer,
        // Wizard container
        WizardContainer
        };

    enum AuxiliaryItemDataRoles {
        // item->flags while being edited
        ItemFlagsShadowRole = 0x13370551
    };

}

QT_END_NAMESPACE

#endif // SHAREDENUMS_H
