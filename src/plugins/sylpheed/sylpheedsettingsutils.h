/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef SYLPHEEDSETTINGSUTILS_H
#define SYLPHEEDSETTINGSUTILS_H

#include <KConfigGroup>
#include <QString>

namespace SylpheedSettingsUtils
{
bool readConfig(const QString &key, const KConfigGroup &accountConfig, int &value, bool remove_underscore);
bool readConfig(const QString &key, const KConfigGroup &accountConfig, QString &value, bool remove_underscore);
}

#endif // SYLPHEEDSETTINGSUTILS_H
