/*
   SPDX-FileCopyrightText: 2017-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include "abstractimporter.h"

class SeaMonkeyImportData : public LibImportWizard::AbstractImporter
{
public:
    explicit SeaMonkeyImportData(QObject *parent, const QList<QVariant> & = QList<QVariant>());
    ~SeaMonkeyImportData() override;

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
