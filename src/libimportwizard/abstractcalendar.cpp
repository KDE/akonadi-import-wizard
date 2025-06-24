/*
   SPDX-FileCopyrightText: 2012-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "abstractcalendar.h"
using namespace Qt::Literals::StringLiterals;

#include "abstractdisplayinfo.h"
#include <KConfigGroup>
#include <KSharedConfig>
using namespace LibImportWizard;

AbstractCalendar::AbstractCalendar() = default;

AbstractCalendar::~AbstractCalendar() = default;

void AbstractCalendar::addImportInfo(const QString &log)
{
    Q_ASSERT_X(mAbstractDisplayInfo, __FUNCTION__, "mAbstractDisplayInfo is not setting");
    if (mAbstractDisplayInfo) {
        mAbstractDisplayInfo->calendarImportInfo(log);
    }
}

void AbstractCalendar::addImportError(const QString &log)
{
    Q_ASSERT_X(mAbstractDisplayInfo, __FUNCTION__, "mAbstractDisplayInfo is not setting");
    if (mAbstractDisplayInfo) {
        mAbstractDisplayInfo->calendarImportError(log);
    }
}

// eventviewsrc for calendar color for example
void AbstractCalendar::addEvenViewConfig(const QString &groupName, const QString &key, const QString &value)
{
    KSharedConfigPtr eventViewConfig = KSharedConfig::openConfig(u"eventviewsrc"_s);
    KConfigGroup group = eventViewConfig->group(groupName);
    group.writeEntry(key, value);
    group.sync();
}

#include "moc_abstractcalendar.cpp"
