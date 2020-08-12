/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef EVOLUTIONCALENDAR_H
#define EVOLUTIONCALENDAR_H

#include "abstractcalendar.h"

class QDomElement;

class EvolutionCalendar : public LibImportWizard::AbstractCalendar
{
public:
    EvolutionCalendar();
    ~EvolutionCalendar();

    void loadCalendar(const QString &filename);
private:
    void readCalendar(const QDomElement &calendar);
    void extractCalendarInfo(const QString &info);
    QString mCalendarPath;
};

#endif // EVOLUTIONCALENDAR_H
