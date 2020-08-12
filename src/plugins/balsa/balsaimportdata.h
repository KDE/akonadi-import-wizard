/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef BalsaImportData_H
#define BalsaImportData_H

#include <ImportWizard/AbstractImporter>

class BalsaImportData : public LibImportWizard::AbstractImporter
{
public:
    explicit BalsaImportData(QObject *parent, const QList<QVariant> & = QList<QVariant>());
    ~BalsaImportData() override;

    TypeSupportedOptions supportedOption() override;
    bool foundMailer() const override;

    bool importMails() override;
    bool importAddressBook() override;
    bool importSettings() override;
    bool importFilters() override;
    QString name() const override;
};

#endif /* BalsaImportData_H */
