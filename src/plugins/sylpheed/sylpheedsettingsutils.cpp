/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "sylpheedsettingsutils.h"

bool SylpheedSettingsUtils::readConfig(const QString &key, const KConfigGroup &accountConfig, int &value, bool remove_underscore)
{
    QString cleanedKey(key);
    if (remove_underscore) {
        cleanedKey.remove(QLatin1Char('_'));
    }
    const QString useKey = QLatin1String("set_") + cleanedKey;
    if (accountConfig.hasKey(useKey) && (accountConfig.readEntry(useKey, 0) == 1)) {
        value = accountConfig.readEntry(key, 0);
        return true;
    }
    return false;
}

bool SylpheedSettingsUtils::readConfig(const QString &key, const KConfigGroup &accountConfig, QString &value, bool remove_underscore)
{
    QString cleanedKey(key);
    if (remove_underscore) {
        cleanedKey.remove(QLatin1Char('_'));
    }
    const QString useKey = QLatin1String("set_") + cleanedKey;
    if (accountConfig.hasKey(useKey) && (accountConfig.readEntry(useKey, 0) == 1)) {
        value = accountConfig.readEntry(key);
        return true;
    }
    return false;
}
