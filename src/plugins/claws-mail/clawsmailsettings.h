/*
   SPDX-FileCopyrightText: 2012-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

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
