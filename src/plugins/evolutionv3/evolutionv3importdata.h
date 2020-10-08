/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef Evolutionv3ImportData_H
#define Evolutionv3ImportData_H

#include "abstractimporter.h"

class Evolutionv3ImportData : public LibImportWizard::AbstractImporter
{
public:
    explicit Evolutionv3ImportData(QObject *parent, const QList<QVariant> & = QList<QVariant>());
    ~Evolutionv3ImportData() override;

    TypeSupportedOptions supportedOption() override;
    bool foundMailer() const override;

    bool importSettings() override;
    bool importMails() override;
    bool importFilters() override;
    bool importAddressBook() override;
    bool importCalendar() override;

    QString name() const override;
};

#endif /* Evolutionv3ImportData_H */
