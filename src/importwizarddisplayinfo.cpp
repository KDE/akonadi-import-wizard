/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "importwizarddisplayinfo.h"
#include "autodetect/importaddressbookpage.h"
#include "autodetect/importcalendarpage.h"
#include "autodetect/importfilterinfogui.h"
#include "autodetect/importfilterpage.h"
#include "autodetect/importmailpage.h"
#include "autodetect/importsettingpage.h"
#include "importwizard.h"

#include <MailImporter/FilterInfo>
#include <MailImporterAkonadi/FilterImporterAkonadi>

ImportWizardDisplayInfo::ImportWizardDisplayInfo(ImportWizard *wizard)
    : mImportWizard(wizard)
{
    Q_ASSERT_X(mImportWizard, __FUNCTION__, "mImportWizard is null");
}

ImportWizardDisplayInfo::~ImportWizardDisplayInfo() = default;

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
    auto info = new MailImporter::FilterInfo();
    auto infoGui = new ImportFilterInfoGui(mImportWizard->importMailPage());
    info->setFilterInfoGui(infoGui);
    info->clear(); // Clear info from last time
    auto filterImporter = new MailImporter::FilterImporterAkonadi(info);
    filterImporter->setRootCollection(mImportWizard->importMailPage()->selectedCollection());
    filter.setFilterImporter(filterImporter);
    filter.setFilterInfo(info);
}
