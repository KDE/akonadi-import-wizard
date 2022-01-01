/*
   SPDX-FileCopyrightText: 2012-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "abstractcalendar.h"

class QDomElement;

class EvolutionCalendar : public LibImportWizard::AbstractCalendar
{
public:
    EvolutionCalendar();
    ~EvolutionCalendar() override;

    void loadCalendar(const QString &filename);

private:
    void readCalendar(const QDomElement &calendar);
    void extractCalendarInfo(const QString &info);
    QString mCalendarPath;
};

