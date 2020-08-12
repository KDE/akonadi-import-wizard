/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef EVOLUTIONUTIL_H
#define EVOLUTIONUTIL_H
#include <QString>
class QFile;
class QDomDocument;

namespace EvolutionUtil {
bool loadInDomDocument(QFile *file, QDomDocument &doc);
bool loadInDomDocument(const QString &file, QDomDocument &doc);
}

#endif // EVOLUTIONUTIL_H
