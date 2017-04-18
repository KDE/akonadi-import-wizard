/*
   Copyright (C) 2012-2017 Montel Laurent <montel@kde.org>

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

#include "balsaimportdata.h"
#include "balsasettings.h"
#include "balsaaddressbook.h"
#include "mailimporter/filterbalsa.h"
#include "mailimporter/filterinfo.h"
#include "MailCommon/FilterImporterBalsa"
#include "importfilterinfogui.h"
#include "importwizard.h"

#include <KLocalizedString>
#include <kpluginfactory.h>
#include <QDir>

K_PLUGIN_FACTORY_WITH_JSON(BalsaImporterFactory, "balsaimporter.json", registerPlugin<BalsaImportData>();)

BalsaImportData::BalsaImportData(QObject *parent, const QList<QVariant> &)
    : LibImportWizard::AbstractImporter(parent)
{
    mPath = MailImporter::FilterBalsa::defaultSettingsPath();
}

BalsaImportData::~BalsaImportData()
{
}

bool BalsaImportData::foundMailer() const
{
    QDir directory(mPath);
    if (directory.exists()) {
        return true;
    }
    return false;
}

QString BalsaImportData::name() const
{
    return QStringLiteral("Balsa");
}

bool BalsaImportData::importMails()
{
    MailImporter::FilterBalsa balsa;
    initializeFilter(balsa);
    balsa.filterInfo()->setStatusMessage(i18n("Import in progress"));
    QDir directory(balsa.localMailDirPath());
    if (directory.exists()) {
        balsa.importMails(directory.absolutePath());
    } else {
        balsa.import();
    }
    balsa.filterInfo()->setStatusMessage(i18n("Import finished"));
    return true;
}

bool BalsaImportData::importAddressBook()
{
    const QString addressbookFile(mPath + QStringLiteral("config"));
    BalsaAddressBook addressbook(addressbookFile, mImportWizard);
    addressbook.importAddressBook();
    return true;
}

bool BalsaImportData::importSettings()
{
    const QString settingFile(mPath + QStringLiteral("config"));
    BalsaSettings settings(settingFile, mImportWizard);
    settings.importSettings();
    return true;
}

bool BalsaImportData::importFilters()
{
    const QString filterPath = mPath + QStringLiteral("config");
    return addFilters(filterPath, MailCommon::FilterImporterExporter::BalsaFilter);
}

LibImportWizard::AbstractImporter::TypeSupportedOptions BalsaImportData::supportedOption()
{
    TypeSupportedOptions options;
    options |= LibImportWizard::AbstractImporter::Mails;
    options |= LibImportWizard::AbstractImporter::AddressBooks;
    options |= LibImportWizard::AbstractImporter::Settings;
    options |= LibImportWizard::AbstractImporter::Filters;
    return options;
}

#include "balsaimportdata.moc"