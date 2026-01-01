/*
   SPDX-FileCopyrightText: 2012-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "clawsmailimportdata.h"
using namespace Qt::Literals::StringLiterals;

#include "clawsmailaddressbook.h"
#include "clawsmailsettings.h"
#include <MailCommon/FilterImporterClawsMail>
#include <MailImporter/FilterClawsMail>
#include <MailImporter/FilterInfo>

#include <KLocalizedString>
#include <KPluginFactory>

#include <QDir>

K_PLUGIN_CLASS_WITH_JSON(ClawsMailImportData, "clawsmailimporter.json")

ClawsMailImportData::ClawsMailImportData(QObject *parent, const QList<QVariant> &)
    : LibImportWizard::AbstractImporter(parent)
{
    mPath = MailImporter::FilterClawsMail::defaultSettingsPath();
}

ClawsMailImportData::~ClawsMailImportData() = default;

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
    return u"Claws Mails"_s;
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
    const QDir addressbookDir(mPath + QLatin1StringView("addrbook/"));
    ClawsMailAddressBook account(addressbookDir);
    account.setAbstractDisplayInfo(mAbstractDisplayInfo);
    account.importAddressBook();
    return true;
}

bool ClawsMailImportData::importSettings()
{
    const QString accountFile = mPath + QLatin1StringView("accountrc");
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
    const QString filterPath(mPath + QLatin1StringView("matcherrc"));
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
