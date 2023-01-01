/*
   SPDX-FileCopyrightText: 2017-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "libimportwizard_export.h"
#include <QString>
class QWidget;

namespace MailImporter
{
class Filter;
}

namespace LibImportWizard
{
class LIBIMPORTWIZARD_EXPORT AbstractDisplayInfo
{
public:
    AbstractDisplayInfo();
    virtual ~AbstractDisplayInfo() = default;

    virtual void calendarImportError(const QString &log);
    virtual void settingsImportError(const QString &log);
    virtual void addressbookImportError(const QString &log);
    virtual void filterImportError(const QString &log);

    virtual void calendarImportInfo(const QString &log);
    virtual void settingsImportInfo(const QString &log);
    virtual void addressbookImportInfo(const QString &log);
    virtual void filterImportInfo(const QString &log);

    virtual QWidget *parentWidget() const;
    virtual void initializeFilter(MailImporter::Filter &filter);
};
}
