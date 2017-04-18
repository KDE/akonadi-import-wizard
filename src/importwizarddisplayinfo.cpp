/*
   Copyright (C) 2017 Montel Laurent <montel@kde.org>

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

#include "importwizarddisplayinfo.h"
#include "importwizard.h"
#include "importcalendarpage.h"
#include "importsettingpage.h"
#include "importaddressbookpage.h"
#include "importfilterpage.h"
#include "importmailpage.h"

#include <MailImporter/FilterInfo>
#include <MailImporterAkonadi/FilterImporterAkonadi>
#include <importfilterinfogui.h>

ImportWizardDisplayInfo::ImportWizardDisplayInfo(ImportWizard *wizard)
    : mImportWizard(wizard)
{
}

ImportWizardDisplayInfo::~ImportWizardDisplayInfo()
{
}

void ImportWizardDisplayInfo::calendarImportError(const QString &log)
{
    mImportWizard->importCalendarPage()->addImportError(log);
}

void ImportWizardDisplayInfo::settingsImportError(const QString &log)
{
    mImportWizard->importSettingPage()->addImportError(log);
}

void ImportWizardDisplayInfo::addressbookImportError(const QString &log)
{
    mImportWizard->importAddressBookPage()->addImportError(log);
}

void ImportWizardDisplayInfo::calendarImportInfo(const QString &log)
{
    mImportWizard->importCalendarPage()->addImportInfo(log);
}

void ImportWizardDisplayInfo::settingsImportInfo(const QString &log)
{
    mImportWizard->importSettingPage()->addImportInfo(log);
}

void ImportWizardDisplayInfo::addressbookImportInfo(const QString &log)
{
    mImportWizard->importAddressBookPage()->addImportInfo(log);
}

QWidget *ImportWizardDisplayInfo::parentWidget() const
{
    return mImportWizard;
}

void ImportWizardDisplayInfo::filterImportError(const QString &log)
{
    mImportWizard->importFilterPage()->addImportError(log);
}

void ImportWizardDisplayInfo::filterImportInfo(const QString &log)
{
    mImportWizard->importFilterPage()->addImportInfo(log);
}

void ImportWizardDisplayInfo::initializeFilter(MailImporter::Filter &filter)
{
    if (mImportWizard) {
        MailImporter::FilterInfo *info = new MailImporter::FilterInfo();
        ImportFilterInfoGui *infoGui = new ImportFilterInfoGui(mImportWizard->importMailPage());
        info->setFilterInfoGui(infoGui);
        info->clear(); // Clear info from last time
        MailImporter::FilterImporterAkonadi *filterImporter = new MailImporter::FilterImporterAkonadi(info);
        filterImporter->setRootCollection(mImportWizard->importMailPage()->selectedCollection());
        filter.setFilterImporter(filterImporter);
        filter.setFilterInfo(info);
    }
}
