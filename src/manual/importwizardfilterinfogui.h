/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef IMPORTWIZARDFILTERINFOGUI_H
#define IMPORTWIZARDFILTERINFOGUI_H

#include "filterinfogui.h"
#include "manualimportmailpage.h"

class ImportWizardFilterInfoGui : public MailImporter::FilterInfoGui
{
public:
    explicit ImportWizardFilterInfoGui(ManualImportMailPage *dlg, QWidget *parent);
    ~ImportWizardFilterInfoGui() override;

    void setStatusMessage(const QString &status) override;
    void setFrom(const QString &from) override;
    void setTo(const QString &to) override;
    void setCurrent(const QString &current) override;
    void setCurrent(int percent = 0) override;
    void setOverall(int percent = 0) override;
    void addErrorLogEntry(const QString &log) override;
    void addInfoLogEntry(const QString &log) override;
    void clear() override;
    void alert(const QString &message) override;
    QWidget *parent() const override;

private:
    QWidget *const m_parent;
    ManualImportMailPage *const mManualImportMailPage;
};

#endif /* IMPORTWIZARDFILTERINFOGUI_H */
