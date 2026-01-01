/*
   SPDX-FileCopyrightText: 2012-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "abstractimporter.h"

class IcedoveImportData : public LibImportWizard::AbstractImporter
{
public:
    explicit IcedoveImportData(QObject *parent, const QList<QVariant> & = QList<QVariant>());
    ~IcedoveImportData() override;

    TypeSupportedOptions supportedOption() override;
    bool foundMailer() const override;

    bool importSettings() override;
    bool importMails() override;
    bool importFilters() override;
    bool importAddressBook() override;
    QString name() const override;

private:
    QString defaultProfile();

    QString mDefaultProfile;
};
