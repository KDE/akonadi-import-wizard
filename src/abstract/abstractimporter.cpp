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

#include "abstractimporter.h"
#include "importmailpage.h"
#include "importfilterpage.h"
#include "importsettingpage.h"
#include "importcalendarpage.h"
#include "abstract/abstractdisplayinfo.h"

#include "mailimporter/filterinfo.h"
#include "MailCommon/FilterManager"
#include <MailImporterAkonadi/FilterImporterAkonadi>

#include <QFileInfo>
#include <QFile>

using namespace LibImportWizard;

AbstractImporter::AbstractImporter(QObject *parent)
    : QObject(parent)
    , mAbstractDisplayInfo(nullptr)
    , mParentWidget(nullptr)
{
}

AbstractImporter::~AbstractImporter()
{
}

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
    mAbstractDisplayInfo->initializeFilter(filter);
}

bool AbstractImporter::addFilters(const QString &filterPath, MailCommon::FilterImporterExporter::FilterType type)
{
    if (QFileInfo::exists(filterPath)) {
        bool canceled = false;
        MailCommon::FilterImporterExporter importer(mAbstractDisplayInfo->parentWidget());
        QList<MailCommon::MailFilter *> listFilter = importer.importFilters(canceled, type, filterPath);
        appendFilters(listFilter);
        if (canceled) {
            addImportFilterInfo(i18n("Importing of filters was canceled."));
        } else {
            addImportFilterInfo(i18np("1 filter was imported from \"%2\"", "%1 filters were imported from \"%2\"", listFilter.count(), filterPath));
        }
        return true;
    } else {
        addImportFilterError(i18n("Filters file was not found"));
        return true;
    }
}

void AbstractImporter::appendFilters(const QList<MailCommon::MailFilter *> &filters)
{
    if (!filters.isEmpty()) {
        MailCommon::FilterManager::instance()->appendFilters(filters, false);
    }
}

void AbstractImporter::addImportFilterInfo(const QString &log) const
{
    if (mAbstractDisplayInfo) {
        mAbstractDisplayInfo->filterImportInfo(log);
    }
}

void AbstractImporter::addImportFilterError(const QString &log) const
{
    if (mAbstractDisplayInfo) {
        mAbstractDisplayInfo->filterImportError(log);
    }
}

void AbstractImporter::addImportSettingsInfo(const QString &log) const
{
    if (mAbstractDisplayInfo) {
        mAbstractDisplayInfo->settingsImportInfo(log);
    }
}

void AbstractImporter::addImportCalendarInfo(const QString &log) const
{
    if (mAbstractDisplayInfo) {
        mAbstractDisplayInfo->calendarImportInfo(log);
    }
}

void AbstractImporter::setParentWidget(QWidget *parent)
{
    mParentWidget = parent;
}

QWidget *AbstractImporter::parentWidget() const
{
    return mParentWidget;
}

void AbstractImporter::setAbstractDisplayInfo(AbstractDisplayInfo *info)
{
    mAbstractDisplayInfo = info;
}
