/*
   SPDX-FileCopyrightText: 2012-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "evolutionutil.h"

#include "evolutionv3plugin_debug.h"

#include <QDomDocument>
#include <QFile>

bool EvolutionUtil::loadInDomDocument(QFile *file, QDomDocument &doc)
{
    const QDomDocument::ParseResult parseResult = doc.setContent(file);
    if (!parseResult) {
        qCDebug(EVOLUTIONPLUGIN_LOG) << "Unable to load document.Parse error in line " << parseResult.errorLine << ", col " << parseResult.errorColumn << ": "
                                     << qPrintable(parseResult.errorMessage);
        return false;
    }
    return true;
}

bool EvolutionUtil::loadInDomDocument(const QString &file, QDomDocument &doc)
{
    const QDomDocument::ParseResult parseResult = doc.setContent(file);
    if (!parseResult) {
        qCDebug(EVOLUTIONPLUGIN_LOG) << "Unable to load document.Parse error in line " << parseResult.errorLine << ", col " << parseResult.errorColumn << ": "
                                     << qPrintable(parseResult.errorMessage);
        return false;
    }
    return true;
}
