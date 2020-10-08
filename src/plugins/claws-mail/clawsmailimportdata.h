/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef ClawsMailImportData_H
#define ClawsMailImportData_H

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

#endif /* ClawsMailImportData_H */
