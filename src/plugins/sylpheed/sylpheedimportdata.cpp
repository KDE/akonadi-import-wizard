/*
   Copyright (C) 2012-2019 Montel Laurent <montel@kde.org>

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

#include "sylpheedimportdata.h"
#include "mailimporter/filtersylpheed.h"
#include "mailimporter/filterinfo.h"

#include "MailCommon/FilterImporterExporter"

#include "sylpheedsettings.h"
#include "sylpheedaddressbook.h"

#include <KLocalizedString>
#include <KPluginFactory>
#include <QDir>

K_PLUGIN_CLASS_WITH_JSON(SylpheedImportData, "sylpheedimporter.json")
SylpheedImportData::SylpheedImportData(QObject *parent, const QList<QVariant> &)
    : LibImportWizard::AbstractImporter(parent)
{
    mPath = MailImporter::FilterSylpheed::defaultSettingsPath();
}

SylpheedImportData::~SylpheedImportData()
{
}

bool SylpheedImportData::foundMailer() const
{
    QDir directory(mPath);
    if (directory.exists()) {
        return true;
    }
    return false;
}

QString SylpheedImportData::name() const
{
    return QStringLiteral("Sylpheed");
}

bool SylpheedImportData::importSettings()
{
    const QString accountFile = mPath + QLatin1String("/accountrc");
    if (QFileInfo::exists(accountFile)) {
        SylpheedSettings account;
        account.setAbstractDisplayInfo(mAbstractDisplayInfo);
        account.importSettings(accountFile, mPath);
    } else {
        addImportSettingsInfo(i18n("Sylpheed settings not found."));
    }
    return true;
}

bool SylpheedImportData::importMails()
{
    MailImporter::FilterSylpheed sylpheed;
    initializeFilter(sylpheed);
    sylpheed.filterInfo()->setStatusMessage(i18n("Import in progress"));
    const QString mailsPath = sylpheed.localMailDirPath();
    QDir directory(mailsPath);
    if (directory.exists()) {
        sylpheed.importMails(mailsPath);
    } else {
        sylpheed.import();
    }
    sylpheed.filterInfo()->setStatusMessage(i18n("Import finished"));
    return true;
}

bool SylpheedImportData::importFilters()
{
    const QString filterPath = mPath + QLatin1String("/filter.xml");
    return addFilters(filterPath, MailCommon::FilterImporterExporter::SylpheedFilter);
}

bool SylpheedImportData::importAddressBook()
{
    const QDir addressbookDir(mPath);
    SylpheedAddressBook account(addressbookDir);
    account.setAbstractDisplayInfo(mAbstractDisplayInfo);
    account.importAddressBook();
    return true;
}

LibImportWizard::AbstractImporter::TypeSupportedOptions SylpheedImportData::supportedOption()
{
    TypeSupportedOptions options;
    options |= LibImportWizard::AbstractImporter::Mails;
    options |= LibImportWizard::AbstractImporter::Filters;
    options |= LibImportWizard::AbstractImporter::Settings;
    options |= LibImportWizard::AbstractImporter::AddressBooks;
    return options;
}

#include "sylpheedimportdata.moc"
