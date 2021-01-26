/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ClawsMailSettings_H
#define ClawsMailSettings_H

#include "../sylpheed/sylpheedsettings.h"

class KConfigGroup;

class ClawsMailSettings : public SylpheedSettings
{
public:
    ClawsMailSettings();
    ~ClawsMailSettings() override;

    void importSettings(const QString &filename, const QString &path) override;

protected:
    // Reimplement from sylpheed
    void readSettingsColor(const KConfigGroup &group) override;
    void readTemplateFormat(const KConfigGroup &group) override;
    void readGlobalSettings(const KConfigGroup &group) override;
    void readTagColor(const KConfigGroup &group) override;

private:
    QString writeColor(const QColor &col);
};

#endif // ClawsMailSettings_H
