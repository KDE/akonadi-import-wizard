/*
   SPDX-FileCopyrightText: 2012-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "selectcomponentpage.h"
#include "ui_selectcomponentpage.h"

SelectComponentPage::SelectComponentPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SelectComponentPage)
{
    ui->setupUi(this);
    connect(ui->everything, &QCheckBox::clicked, this, &SelectComponentPage::slotEverythingClicked);
    connect(ui->addressbooks, &QCheckBox::clicked, this, &SelectComponentPage::slotComponentClicked);
    connect(ui->filters, &QCheckBox::clicked, this, &SelectComponentPage::slotComponentClicked);
    connect(ui->mails, &QCheckBox::clicked, this, &SelectComponentPage::slotComponentClicked);
    connect(ui->settings, &QCheckBox::clicked, this, &SelectComponentPage::slotComponentClicked);
    connect(ui->calendars, &QCheckBox::clicked, this, &SelectComponentPage::slotComponentClicked);
}

SelectComponentPage::~SelectComponentPage()
{
    delete ui;
}

void SelectComponentPage::slotComponentClicked()
{
    const bool componentSelected = (ui->addressbooks->isChecked() || ui->filters->isChecked() || ui->mails->isChecked() || ui->settings->isChecked()
                                    || ui->calendars->isChecked() || ui->everything->isChecked());
    Q_EMIT atLeastOneComponentSelected(componentSelected);
}

void SelectComponentPage::slotEverythingClicked(bool clicked)
{
    ui->addressbooks->setEnabled(!clicked && (mOptions & LibImportWizard::AbstractImporter::AddressBooks));
    ui->filters->setEnabled(!clicked && (mOptions & LibImportWizard::AbstractImporter::Filters));
    ui->mails->setEnabled(!clicked && (mOptions & LibImportWizard::AbstractImporter::Mails));
    ui->settings->setEnabled(!clicked && (mOptions & LibImportWizard::AbstractImporter::Settings));
    ui->calendars->setEnabled(!clicked && (mOptions & LibImportWizard::AbstractImporter::Calendars));
    slotComponentClicked();
}

void SelectComponentPage::setEnabledComponent(LibImportWizard::AbstractImporter::TypeSupportedOptions options)
{
    mOptions = options;
    slotEverythingClicked(ui->everything->isChecked());
}

LibImportWizard::AbstractImporter::TypeSupportedOptions SelectComponentPage::selectedComponents() const
{
    if (ui->everything->isChecked()) {
        return mOptions;
    } else {
        LibImportWizard::AbstractImporter::TypeSupportedOptions newOptions;
        if (ui->addressbooks->isChecked()) {
            newOptions |= LibImportWizard::AbstractImporter::AddressBooks;
        }
        if (ui->filters->isChecked()) {
            newOptions |= LibImportWizard::AbstractImporter::Filters;
        }
        if (ui->mails->isChecked()) {
            newOptions |= LibImportWizard::AbstractImporter::Mails;
        }
        if (ui->settings->isChecked()) {
            newOptions |= LibImportWizard::AbstractImporter::Settings;
        }
        if (ui->calendars->isChecked()) {
            newOptions |= LibImportWizard::AbstractImporter::Calendars;
        }

        return newOptions;
    }
}

#include "moc_selectcomponentpage.cpp"
