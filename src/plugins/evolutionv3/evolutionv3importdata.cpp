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

#include "evolutionv3importdata.h"
#include "evolutionsettings.h"
#include "evolutioncalendar.h"
#include "evolutionaddressbook.h"
#include "mailimporter/filterevolution_v3.h"
#include "mailimporter/filterinfo.h"
#include "importfilterinfogui.h"
#include "MailCommon/FilterImporterExporter"
#include <kpluginfactory.h>
#include "importwizard.h"

#include <KLocalizedString>

#include <QDir>

K_PLUGIN_FACTORY_WITH_JSON(Evolutionv3ImporterFactory, "evolutionv3importer.json", registerPlugin<Evolutionv3ImportData>();)


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
        EvolutionSettings account(mImportWizard);
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
    EvolutionAddressBook addressbook(mImportWizard);
    addressbook.exportEvolutionAddressBook();
    return true;
}

bool Evolutionv3ImportData::importCalendar()
{
    const QString calendarFile = QDir::homePath() + QLatin1String("/.gconf/apps/evolution/calendar/%gconf.xml");
    if (QFileInfo::exists(calendarFile)) {
        EvolutionCalendar calendar(mImportWizard);
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
