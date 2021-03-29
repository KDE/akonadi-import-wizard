/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

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

