/*
   SPDX-FileCopyrightText: 2012-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "abstractimporter.h"

class TrojitaImportData : public LibImportWizard::AbstractImporter
{
public:
    explicit TrojitaImportData(QObject *parent, const QList<QVariant> & = QList<QVariant>());
    ~TrojitaImportData() override;

    TypeSupportedOptions supportedOption() override;
    bool foundMailer() const override;

    bool importMails() override;
    bool importSettings() override;
    bool importAddressBook() override;

    QString name() const override;
};
