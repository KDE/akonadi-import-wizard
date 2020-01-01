/*
   Copyright (C) 2012-2020 Laurent Montel <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
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
    //Reimplement from sylpheed
    void readSettingsColor(const KConfigGroup &group) override;
    void readTemplateFormat(const KConfigGroup &group) override;
    void readGlobalSettings(const KConfigGroup &group) override;
    void readTagColor(const KConfigGroup &group) override;

private:
    QString writeColor(const QColor &col);
};

#endif // ClawsMailSettings_H
