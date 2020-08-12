/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef SylpheedImportData_H
#define SylpheedImportData_H

#include <ImportWizard/AbstractImporter>

class SylpheedImportData : public LibImportWizard::AbstractImporter
{
public:
    explicit SylpheedImportData(QObject *parent, const QList<QVariant> & = QList<QVariant>());
    ~SylpheedImportData() override;

    TypeSupportedOptions supportedOption() override;
    bool foundMailer() const override;

    bool importSettings() override;
    bool importMails() override;
    bool importFilters() override;
    bool importAddressBook() override;
    QString name() const override;
};

#endif /* SylpheedImportData_H */
