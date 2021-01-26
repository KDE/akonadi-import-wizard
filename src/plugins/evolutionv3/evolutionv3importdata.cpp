/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "evolutionv3importdata.h"
#include "evolutionaddressbook.h"
#include "evolutioncalendar.h"
#include "evolutionsettings.h"
#include <MailImporter/FilterEvolution_v3>
#include <MailImporter/FilterInfo>

#include <KPluginFactory>
#include <MailCommon/FilterImporterExporter>

#include <KLocalizedString>

#include <QDir>

K_PLUGIN_CLASS_WITH_JSON(Evolutionv3ImportData, "evolutionv3importer.json")

Evolutionv3ImportData::Evolutionv3ImportData(QObject *parent, const QList<QVariant> &)
    : LibImportWizard::AbstractImporter(parent)
{
    mPath = MailImporter::FilterEvolution_v3::defaultSettingsPath();
}

Evolutionv3ImportData::~Evolutionv3ImportData()
{
}

bool Evolutionv3ImportData::foundMailer() const
{
    QDir directory(mPath);
    if (directory.exists()) {
        return true;
    }
    return false;
}

QString Evolutionv3ImportData::name() const
{
    return QStringLiteral("Evolution 3.x");
}

bool Evolutionv3ImportData::importSettings()
{
    const QString accountFile = QDir::homePath() + QLatin1String("/.gconf/apps/evolution/mail/%gconf.xml");
    if (QFileInfo::exists(accountFile)) {
        EvolutionSettings account;
        account.setAbstractDisplayInfo(mAbstractDisplayInfo);
        account.loadAccount(accountFile);
        const QString ldapFile = QDir::homePath() + QLatin1String("/.gconf/apps/evolution/addressbook/%gconf.xml");
        if (QFileInfo::exists(ldapFile)) {
            account.loadLdap(ldapFile);
        }
    } else {
        addImportSettingsInfo(i18n("Evolution settings not found."));
    }
    return true;
}

bool Evolutionv3ImportData::importMails()
{
    MailImporter::FilterEvolution_v3 evolution;
    initializeFilter(evolution);
    evolution.filterInfo()->setStatusMessage(i18n("Import in progress"));
    const QString mailsPath = mPath;
    QDir directory(mailsPath);
    if (directory.exists()) {
        evolution.importMails(mailsPath);
    } else {
        evolution.import();
    }
    evolution.filterInfo()->setStatusMessage(i18n("Import finished"));
    return true;
}

bool Evolutionv3ImportData::importFilters()
{
    const QString filterPath = QDir::homePath() + QLatin1String("/.config/evolution/mail/filter.xml");
    return addFilters(filterPath, MailCommon::FilterImporterExporter::EvolutionFilter);
}

bool Evolutionv3ImportData::importAddressBook()
{
    EvolutionAddressBook addressbook;
    addressbook.setAbstractDisplayInfo(mAbstractDisplayInfo);
    addressbook.exportEvolutionAddressBook();
    return true;
}

bool Evolutionv3ImportData::importCalendar()
{
    const QString calendarFile = QDir::homePath() + QLatin1String("/.gconf/apps/evolution/calendar/%gconf.xml");
    if (QFileInfo::exists(calendarFile)) {
        EvolutionCalendar calendar;
        calendar.setAbstractDisplayInfo(mAbstractDisplayInfo);
        calendar.loadCalendar(calendarFile);
    } else {
        addImportCalendarInfo(i18n("Evolution calendar not found."));
    }
    return true;
}

LibImportWizard::AbstractImporter::TypeSupportedOptions Evolutionv3ImportData::supportedOption()
{
    TypeSupportedOptions options;
    options |= LibImportWizard::AbstractImporter::Mails;
    options |= LibImportWizard::AbstractImporter::Filters;
    options |= LibImportWizard::AbstractImporter::Settings;
    options |= LibImportWizard::AbstractImporter::Calendars;
    options |= LibImportWizard::AbstractImporter::AddressBooks;
    return options;
}

#include "evolutionv3importdata.moc"
