/*
   SPDX-FileCopyrightText: 2012-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "sylpheedimportdata.h"
using namespace Qt::Literals::StringLiterals;

#include <MailImporter/FilterInfo>
#include <MailImporter/FilterSylpheed>

#include <MailCommon/FilterImporterExporter>

#include "sylpheedaddressbook.h"
#include "sylpheedsettings.h"

#include <KLocalizedString>
#include <KPluginFactory>
#include <QDir>

K_PLUGIN_CLASS_WITH_JSON(SylpheedImportData, "sylpheedimporter.json")
SylpheedImportData::SylpheedImportData(QObject *parent, const QList<QVariant> &)
    : LibImportWizard::AbstractImporter(parent)
{
    mPath = MailImporter::FilterSylpheed::defaultSettingsPath();
}

SylpheedImportData::~SylpheedImportData() = default;

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
    return u"Sylpheed"_s;
}

bool SylpheedImportData::importSettings()
{
    const QString accountFile = mPath + QLatin1StringView("/accountrc");
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
    const QString filterPath = mPath + QLatin1StringView("/filter.xml");
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
