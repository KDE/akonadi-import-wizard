/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "balsaimportdata.h"
#include "balsaaddressbook.h"
#include "balsasettings.h"
#include <MailCommon/FilterImporterBalsa>
#include <MailImporter/FilterBalsa>
#include <MailImporter/FilterInfo>

#include <KLocalizedString>
#include <KPluginFactory>
#include <QDir>

K_PLUGIN_CLASS_WITH_JSON(BalsaImportData, "balsaimporter.json")

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
    BalsaAddressBook addressbook(addressbookFile);
    addressbook.setAbstractDisplayInfo(mAbstractDisplayInfo);
    addressbook.importAddressBook();
    return true;
}

bool BalsaImportData::importSettings()
{
    const QString settingFile(mPath + QStringLiteral("config"));
    BalsaSettings settings(settingFile);
    settings.setAbstractDisplayInfo(mAbstractDisplayInfo);
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
