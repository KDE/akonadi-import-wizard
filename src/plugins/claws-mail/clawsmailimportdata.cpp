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

#include "clawsmailimportdata.h"
#include "clawsmailsettings.h"
#include "clawsmailaddressbook.h"
#include "mailimporter/filterclawsmail.h"
#include "MailCommon/FilterImporterClawsMail"
#include "mailimporter/filterinfo.h"
#include "importfilterinfogui.h"
#include "importwizard.h"

#include <kpluginfactory.h>
#include <KLocalizedString>

#include <QDir>

K_PLUGIN_FACTORY_WITH_JSON(ClawsMailImporterFactory, "clawsmailimporter.json", registerPlugin<ClawsMailImportData>();
                           )

ClawsMailImportData::ClawsMailImportData(QObject *parent, const QList<QVariant> &)
    : LibImportWizard::AbstractImporter(parent)
{
    mPath = MailImporter::FilterClawsMail::defaultSettingsPath();
}

ClawsMailImportData::~ClawsMailImportData()
{
}

bool ClawsMailImportData::foundMailer() const
{
    QDir directory(mPath);
    if (directory.exists()) {
        return true;
    }
    return false;
}

QString ClawsMailImportData::name() const
{
    return QStringLiteral("Claws Mails");
}

bool ClawsMailImportData::importMails()
{
    MailImporter::FilterClawsMail clawsMail;
    initializeFilter(clawsMail);
    clawsMail.filterInfo()->setStatusMessage(i18n("Import in progress"));
    const QString mailsPath = clawsMail.localMailDirPath();
    QDir directory(mailsPath);
    if (directory.exists()) {
        clawsMail.importMails(mailsPath);
    } else {
        clawsMail.import();
    }
    clawsMail.filterInfo()->setStatusMessage(i18n("Import finished"));
    return true;
}

bool ClawsMailImportData::importAddressBook()
{
    const QDir addressbookDir(mPath + QLatin1String("addrbook/"));
    ClawsMailAddressBook account(addressbookDir);
    account.setAbstractDisplayInfo(mAbstractDisplayInfo);
    account.importAddressBook();
    return true;
}

bool ClawsMailImportData::importSettings()
{
    const QString accountFile = mPath + QLatin1String("accountrc");
    if (QFileInfo::exists(accountFile)) {
        ClawsMailSettings account;
        account.setAbstractDisplayInfo(mAbstractDisplayInfo);
        account.importSettings(accountFile, mPath);
    } else {
        addImportSettingsInfo(i18n("Claws Mail settings not found."));
    }
    return true;
}

bool ClawsMailImportData::importFilters()
{
    const QString filterPath(mPath + QLatin1String("matcherrc"));
    return addFilters(filterPath, MailCommon::FilterImporterExporter::ClawsMailFilter);
}

LibImportWizard::AbstractImporter::TypeSupportedOptions ClawsMailImportData::supportedOption()
{
    TypeSupportedOptions options;
    options |= LibImportWizard::AbstractImporter::Mails;
    options |= LibImportWizard::AbstractImporter::AddressBooks;
    options |= LibImportWizard::AbstractImporter::Settings;
    options |= LibImportWizard::AbstractImporter::Filters;
    return options;
}

#include "clawsmailimportdata.moc"
