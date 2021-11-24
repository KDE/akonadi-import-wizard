/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "trojitaimportdata.h"
#include "trojitaaddressbook.h"
#include "trojitasettings.h"
#include <MailImporter/FilterInfo>
#include <MailImporter/OtherMailerUtil>

#include <KPluginFactory>

#include <QDir>

K_PLUGIN_CLASS_WITH_JSON(TrojitaImportData, "trojitaimporter.json")

TrojitaImportData::TrojitaImportData(QObject *parent, const QList<QVariant> &)
    : LibImportWizard::AbstractImporter(parent)
{
    mPath = MailImporter::OtherMailerUtil::trojitaDefaultPath();
}

TrojitaImportData::~TrojitaImportData() = default;

bool TrojitaImportData::foundMailer() const
{
    QDir directory(mPath);
    if (directory.exists()) {
        return true;
    }
    return false;
}

QString TrojitaImportData::name() const
{
    return QStringLiteral("Trojita");
}

bool TrojitaImportData::importMails()
{
    return false;
}

bool TrojitaImportData::importSettings()
{
    const QString settingsPath = mPath + QLatin1String("trojita.conf");
    TrojitaSettings settings(settingsPath);
    settings.setAbstractDisplayInfo(mAbstractDisplayInfo);
    settings.importSettings();
    return true;
}

bool TrojitaImportData::importAddressBook()
{
    const QString addressbookPath = QDir::homePath() + QLatin1String("/.abook/addressbook");
    TrojitaAddressBook addressBooks(addressbookPath);
    addressBooks.setAbstractDisplayInfo(mAbstractDisplayInfo);
    addressBooks.readAddressBook();
    return true;
}

LibImportWizard::AbstractImporter::TypeSupportedOptions TrojitaImportData::supportedOption()
{
    TypeSupportedOptions options;
    // options |=LibImportWizard::AbstractImporter::Mails;
    options |= LibImportWizard::AbstractImporter::Settings;
    options |= LibImportWizard::AbstractImporter::AddressBooks;
    return options;
}

#include "trojitaimportdata.moc"
