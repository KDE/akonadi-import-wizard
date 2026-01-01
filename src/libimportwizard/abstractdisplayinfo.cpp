/*
   SPDX-FileCopyrightText: 2017-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "abstractdisplayinfo.h"

using namespace LibImportWizard;

AbstractDisplayInfo::AbstractDisplayInfo() = default;

void AbstractDisplayInfo::calendarImportError(const QString &log)
{
    Q_UNUSED(log)
}

void AbstractDisplayInfo::settingsImportError(const QString &log)
{
    Q_UNUSED(log)
}

void AbstractDisplayInfo::addressbookImportError(const QString &log)
{
    Q_UNUSED(log)
}

void AbstractDisplayInfo::filterImportError(const QString &log)
{
    Q_UNUSED(log)
}

void AbstractDisplayInfo::calendarImportInfo(const QString &log)
{
    Q_UNUSED(log)
}

void AbstractDisplayInfo::settingsImportInfo(const QString &log)
{
    Q_UNUSED(log)
}

void AbstractDisplayInfo::addressbookImportInfo(const QString &log)
{
    Q_UNUSED(log)
}

void AbstractDisplayInfo::filterImportInfo(const QString &log)
{
    Q_UNUSED(log)
}

QWidget *AbstractDisplayInfo::parentWidget() const
{
    return nullptr;
}

void AbstractDisplayInfo::initializeFilter(MailImporter::Filter &filter)
{
    Q_UNUSED(filter)
}
