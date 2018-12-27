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

#include "thunderbirdimportdata.h"

#include "thunderbirdsettings.h"
#include "thunderbirdaddressbook.h"
#include "abstractdisplayinfo.h"

#include "mailimporter/filterthunderbird.h"
#include "mailimporter/filterinfo.h"
#include "MailCommon/FilterImporterExporter"

#include <KLocalizedString>
#include <kpluginfactory.h>

#include <QDir>

K_PLUGIN_CLASS_WITH_JSON(ThunderbirdImportData, "thunderbirdimporter.json")

ThunderbirdImportData::ThunderbirdImportData(QObject *parent, const QList<QVariant> &)
    : LibImportWizard::AbstractImporter(parent)
{
    mPath = MailImporter::FilterThunderbird::defaultSettingsPath();
}

ThunderbirdImportData::~ThunderbirdImportData()
{
}

QString ThunderbirdImportData::defaultProfile()
{
    if (mDefaultProfile.isEmpty()) {
        mDefaultProfile = MailImporter::FilterThunderbird::defaultProfile(mPath, mAbstractDisplayInfo->parentWidget());
    }
    return mDefaultProfile;
}

bool ThunderbirdImportData::foundMailer() const
{
    QDir directory(mPath);
    if (directory.exists()) {
        return true;
    }
    return false;
}

bool ThunderbirdImportData::importAddressBook()
{
    const QDir addressbookDir(mPath + defaultProfile());
    ThunderBirdAddressBook account(addressbookDir);
    account.setAbstractDisplayInfo(mAbstractDisplayInfo);
    account.importAddressBook();
    return true;
}

QString ThunderbirdImportData::name() const
{
    return QStringLiteral("Thunderbird");
}

bool ThunderbirdImportData::importSettings()
{
    const QString accountFile = mPath + defaultProfile() + QLatin1String("/prefs.js");
    if (QFileInfo::exists(accountFile)) {
        ThunderbirdSettings account(accountFile);
        account.setAbstractDisplayInfo(mAbstractDisplayInfo);
        account.importSettings();
    } else {
        addImportSettingsInfo(i18n("Thunderbird settings not found."));
    }
    return true;
}

bool ThunderbirdImportData::importMails()
{
    //* This should be usually ~/.thunderbird/xxxx.default/Mail/Local Folders/
    MailImporter::FilterThunderbird thunderbird;
    initializeFilter(thunderbird);
    thunderbird.filterInfo()->setStatusMessage(i18n("Import in progress"));
    const QString mailsPath = mPath + defaultProfile() + QLatin1String("/Mail/Local Folders/");
    QDir directory(mailsPath);
    if (directory.exists()) {
        thunderbird.importMails(mailsPath);
    } else {
        thunderbird.import();
    }
    thunderbird.filterInfo()->setStatusMessage(i18n("Import finished"));
    return true;
}

bool ThunderbirdImportData::importFilters()
{
    const QString path(mPath + defaultProfile());
    QDir dir(path);
    bool filtersAdded = false;
    const QStringList subDir = dir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name);
    if (subDir.isEmpty()) {
        return true;
    }

    for (const QString &mailPath : subDir) {
        const QString subMailPath(path + QLatin1Char('/') + mailPath);
        QDir dirMail(subMailPath);
        const QStringList subDirMail = dirMail.entryList(QDir::AllDirs | QDir::NoDotAndDotDot, QDir::Name);
        bool foundFilterFile = false;
        for (const QString &file : subDirMail) {
            const QString filterFile(subMailPath + QLatin1Char('/') + file + QLatin1String("/msgFilterRules.dat"));
            if (QFileInfo::exists(filterFile)) {
                foundFilterFile = true;
                const bool added = addFilters(filterFile, MailCommon::FilterImporterExporter::ThunderBirdFilter);
                if (!filtersAdded && added) {
                    filtersAdded = true;
                }
            }
        }
        if (!foundFilterFile) {
            return true;
        }
    }
    return filtersAdded;
}

LibImportWizard::AbstractImporter::TypeSupportedOptions ThunderbirdImportData::supportedOption()
{
    TypeSupportedOptions options;
    options |= LibImportWizard::AbstractImporter::Mails;
    options |= LibImportWizard::AbstractImporter::Filters;
    options |= LibImportWizard::AbstractImporter::Settings;
    options |= LibImportWizard::AbstractImporter::AddressBooks;
    return options;
}

#include "thunderbirdimportdata.moc"
