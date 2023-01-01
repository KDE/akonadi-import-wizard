/*
   SPDX-FileCopyrightText: 2012-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "importwizardfilterinfogui.h"
#include <KMessageBox>
#include <QApplication>

ImportWizardFilterInfoGui::ImportWizardFilterInfoGui(ManualImportMailPage *dlg, QWidget *parent)
    : MailImporter::FilterInfoGui()
    , m_parent(parent)
    , mManualImportMailPage(dlg)
{
}

ImportWizardFilterInfoGui::~ImportWizardFilterInfoGui() = default;

void ImportWizardFilterInfoGui::setStatusMessage(const QString &status)
{
    mManualImportMailPage->widget()->mMailImporterWidget->setStatusMessage(status);
}

void ImportWizardFilterInfoGui::setFrom(const QString &from)
{
    mManualImportMailPage->widget()->mMailImporterWidget->setFrom(from);
}

void ImportWizardFilterInfoGui::setTo(const QString &to)
{
    mManualImportMailPage->widget()->mMailImporterWidget->setTo(to);
}

void ImportWizardFilterInfoGui::setCurrent(const QString &current)
{
    mManualImportMailPage->widget()->mMailImporterWidget->setCurrent(current);
    qApp->processEvents();
}

void ImportWizardFilterInfoGui::setCurrent(int percent)
{
    mManualImportMailPage->widget()->mMailImporterWidget->setCurrent(percent);
    qApp->processEvents(); // Be careful - back & finish buttons disabled, so only user event that can happen is cancel/close button
}

void ImportWizardFilterInfoGui::setOverall(int percent)
{
    mManualImportMailPage->widget()->mMailImporterWidget->setOverall(percent);
}

void ImportWizardFilterInfoGui::addInfoLogEntry(const QString &log)
{
    mManualImportMailPage->widget()->mMailImporterWidget->addInfoLogEntry(log);
    mManualImportMailPage->widget()->mMailImporterWidget->setLastCurrentItem();
    qApp->processEvents();
}

void ImportWizardFilterInfoGui::addErrorLogEntry(const QString &log)
{
    mManualImportMailPage->widget()->mMailImporterWidget->addErrorLogEntry(log);
    mManualImportMailPage->widget()->mMailImporterWidget->setLastCurrentItem();
    qApp->processEvents();
}

void ImportWizardFilterInfoGui::clear()
{
    mManualImportMailPage->widget()->mMailImporterWidget->clear();
}

void ImportWizardFilterInfoGui::alert(const QString &message)
{
    KMessageBox::information(m_parent, message);
}

QWidget *ImportWizardFilterInfoGui::parent() const
{
    return m_parent;
}
