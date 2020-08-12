/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ABSTRACTCALENDAR_H
#define ABSTRACTCALENDAR_H

#include "abstractbase.h"
#include "libimportwizard_export.h"
namespace LibImportWizard {
class LIBIMPORTWIZARD_EXPORT AbstractCalendar : public LibImportWizard::AbstractBase
{
    Q_OBJECT
public:
    AbstractCalendar();
    ~AbstractCalendar() override;

protected:
    void addEvenViewConfig(const QString &groupName, const QString &key, const QString &value);
    void addImportInfo(const QString &log) override;
    void addImportError(const QString &log) override;
};
}
#endif // ABSTRACTCALENDAR_H
