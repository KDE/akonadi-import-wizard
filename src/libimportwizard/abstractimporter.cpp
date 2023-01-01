/*
   SPDX-FileCopyrightText: 2012-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "abstractimporter.h"
#include "abstractdisplayinfo.h"

#include <MailCommon/FilterManager>
#include <MailImporter/FilterInfo>
#include <MailImporterAkonadi/FilterImporterAkonadi>

#include <QFile>
#include <QFileInfo>

using namespace LibImportWizard;

AbstractImporter::AbstractImporter(QObject *parent)
    : QObject(parent)
{
}

AbstractImporter::~AbstractImporter() = default;

bool AbstractImporter::importSettings()
{
    return false;
}

bool AbstractImporter::importMails()
{
    return false;
}

bool AbstractImporter::importFilters()
{
    return false;
}

bool AbstractImporter::importAddressBook()
{
    return false;
}

bool AbstractImporter::importCalendar()
{
    return false;
}

void AbstractImporter::initializeFilter(MailImporter::Filter &filter)
{
    Q_ASSERT_X(mAbstractDisplayInfo, __FUNCTION__, "mAbstractDisplayInfo is not setting");
    mAbstractDisplayInfo->initializeFilter(filter);
}

bool AbstractImporter::addFilters(const QString &filterPath, MailCommon::FilterImporterExporter::FilterType type)
{
    if (QFileInfo::exists(filterPath)) {
        bool canceled = false;
        MailCommon::FilterImporterExporter importer(mAbstractDisplayInfo->parentWidget());
        QVector<MailCommon::MailFilter *> listFilter = importer.importFilters(canceled, type, filterPath);
        appendFilters(listFilter);
        if (canceled) {
            addImportFilterInfo(i18n("Importing of filters from \"%1\" was canceled.", filterPath));
        } else {
            addImportFilterInfo(i18np("1 filter was imported from \"%2\"", "%1 filters were imported from \"%2\"", listFilter.count(), filterPath));
        }
        return true;
    } else {
        addImportFilterError(i18n("Filters file was not found"));
        return true;
    }
}

void AbstractImporter::appendFilters(const QVector<MailCommon::MailFilter *> &filters)
{
    if (!filters.isEmpty()) {
        MailCommon::FilterManager::instance()->appendFilters(filters, false);
    }
}

void AbstractImporter::addImportFilterInfo(const QString &log) const
{
    Q_ASSERT_X(mAbstractDisplayInfo, __FUNCTION__, "mAbstractDisplayInfo is not setting");
    if (mAbstractDisplayInfo) {
        mAbstractDisplayInfo->filterImportInfo(log);
    }
}

void AbstractImporter::addImportFilterError(const QString &log) const
{
    Q_ASSERT_X(mAbstractDisplayInfo, __FUNCTION__, "mAbstractDisplayInfo is not setting");
    if (mAbstractDisplayInfo) {
        mAbstractDisplayInfo->filterImportError(log);
    }
}

void AbstractImporter::addImportSettingsInfo(const QString &log) const
{
    Q_ASSERT_X(mAbstractDisplayInfo, __FUNCTION__, "mAbstractDisplayInfo is not setting");
    if (mAbstractDisplayInfo) {
        mAbstractDisplayInfo->settingsImportInfo(log);
    }
}

void AbstractImporter::addImportCalendarInfo(const QString &log) const
{
    Q_ASSERT_X(mAbstractDisplayInfo, __FUNCTION__, "mAbstractDisplayInfo is not setting");
    if (mAbstractDisplayInfo) {
        mAbstractDisplayInfo->calendarImportInfo(log);
    }
}

void AbstractImporter::setAbstractDisplayInfo(AbstractDisplayInfo *info)
{
    mAbstractDisplayInfo = info;
}
