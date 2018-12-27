/*
   Copyright (C) 2017-2019 Montel Laurent <montel@kde.org>

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

#ifndef IMPORTWIZARDDISPLAYINFO_H
#define IMPORTWIZARDDISPLAYINFO_H

#include "abstractdisplayinfo.h"

class ImportWizard;
class ImportWizardDisplayInfo : public LibImportWizard::AbstractDisplayInfo
{
public:
    explicit ImportWizardDisplayInfo(ImportWizard *wizard);
    ~ImportWizardDisplayInfo() override;

    void calendarImportError(const QString &log) override;
    void settingsImportError(const QString &log) override;
    void addressbookImportError(const QString &log) override;
    void calendarImportInfo(const QString &log) override;
    void settingsImportInfo(const QString &log) override;
    void addressbookImportInfo(const QString &log) override;
    void filterImportError(const QString &log) override;
    void filterImportInfo(const QString &log) override;

    void initializeFilter(MailImporter::Filter &filter) override;

    QWidget *parentWidget() const override;
private:
    ImportWizard *mImportWizard = nullptr;
};

#endif // IMPORTWIZARDDISPLAYINFO_H
