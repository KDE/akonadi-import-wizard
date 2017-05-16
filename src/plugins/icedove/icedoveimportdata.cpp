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

#include "icedoveimportdata.h"

#include "abstractdisplayinfo.h"
#include "../thunderbird/thunderbirdsettings.h"
#include "../thunderbird/thunderbirdaddressbook.h"

#include "mailimporter/filtericedove.h"
#include "mailimporter/filterinfo.h"
#include "MailCommon/FilterImporterExporter"

#include <KLocalizedString>
#include <kpluginfactory.h>
#include <KConfig>

#include <QDir>

IcedoveImportData::IcedoveImportData(QObject *parent, const QList<QVariant> &)
    : LibImportWizard::AbstractImporter(parent)
{
    mPath = MailImporter::FilterIcedove::defaultSettingsPath();
}

IcedoveImportData::~IcedoveImportData()
{
}

QString IcedoveImportData::defaultProfile()
{
    if (mDefaultProfile.isEmpty()) {
        mDefaultProfile = MailImporter::FilterThunderbird::defaultProfile(mPath, mAbstractDisplayInfo->parentWidget());
    }
    return mDefaultProfile;
}

bool IcedoveImportData::foundMailer() const
{
    QDir directory(mPath);
    if (directory.exists()) {
        return true;
    }
    return false;
}

bool IcedoveImportData::importAddressBook()
{
    const QDir addressbookDir(mPath + defaultProfile());
    ThunderBirdAddressBook account(addressbookDir);
    account.setAbstractDisplayInfo(mAbstractDisplayInfo);
    account.importAddressBook();
    return true;
}

QString IcedoveImportData::name() const
{
    return QStringLiteral("Icedove");
}

bool IcedoveImportData::importSettings()
{
    const QString accountFile = mPath + defaultProfile() + QLatin1String("/prefs.js");
    if (QFileInfo::exists(accountFile)) {
        ThunderbirdSettings account(accountFile);
        account.setAbstractDisplayInfo(mAbstractDisplayInfo);
        account.importSettings();
    } else {
        addImportSettingsInfo(i18n("Icedove settings not found."));
    }
    return true;
}

bool IcedoveImportData::importMails()
{
    //* This should be usually ~/.icedove/xxxx.default/Mail/Local Folders/
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

bool IcedoveImportData::importFilters()
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
                const bool added = addFilters(filterFile, MailCommon::FilterImporterExporter::IcedoveFilter);
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

LibImportWizard::AbstractImporter::TypeSupportedOptions IcedoveImportData::supportedOption()
{
    TypeSupportedOptions options;
    options |= LibImportWizard::AbstractImporter::Mails;
    options |= LibImportWizard::AbstractImporter::Filters;
    options |= LibImportWizard::AbstractImporter::Settings;
    options |= LibImportWizard::AbstractImporter::AddressBooks;
    return options;
}
