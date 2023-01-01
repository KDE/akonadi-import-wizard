/*
   SPDX-FileCopyrightText: 2017-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "abstractdisplayinfo.h"

class ImportWizard;
class ImportWizardDisplayInfo : public LibImportWizard::AbstractDisplayInfo
{
public:
    explicit ImportWizardDisplayInfo(ImportWizard *wizard);
    ~ImportWizardDisplayInfo() override;

    void calendarImportError(const QString &log) override;
    void settingsImportError(const QString &log) override;
    void addressbookImportError(const QString &log) override;
    void calendarImportInfo(const QString &log) override;
    void settingsImportInfo(const QString &log) override;
    void addressbookImportInfo(const QString &log) override;
    void filterImportError(const QString &log) override;
    void filterImportInfo(const QString &log) override;

    void initializeFilter(MailImporter::Filter &filter) override;

    QWidget *parentWidget() const override;

private:
    ImportWizard *const mImportWizard;
};
