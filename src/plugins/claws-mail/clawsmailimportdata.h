/*
   SPDX-FileCopyrightText: 2012-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "abstractimporter.h"

class ClawsMailImportData : public LibImportWizard::AbstractImporter
{
public:
    explicit ClawsMailImportData(QObject *parent, const QList<QVariant> & = QList<QVariant>());
    ~ClawsMailImportData() override;

    TypeSupportedOptions supportedOption() override;
    bool foundMailer() const override;

    bool importMails() override;
    bool importAddressBook() override;
    bool importSettings() override;
    bool importFilters() override;

    QString name() const override;
};
