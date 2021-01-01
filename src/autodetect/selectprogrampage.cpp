/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "selectprogrampage.h"
#include "ui_selectprogrampage.h"

SelectProgramPage::SelectProgramPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::SelectProgramPage)
{
    ui->setupUi(this);
    connect(ui->listProgramFound, &QListWidget::itemSelectionChanged, this, &SelectProgramPage::slotItemSelectionChanged);
    connect(ui->listProgramFound, &QListWidget::itemDoubleClicked, this, &SelectProgramPage::slotItemDoubleClicked);
    connect(ui->manualSelectionCheckBox, &QCheckBox::clicked, this, &SelectProgramPage::slotSelectManualSelectionChanged);
}

SelectProgramPage::~SelectProgramPage()
{
    delete ui;
}

void SelectProgramPage::setFoundProgram(const QStringList &list)
{
    ui->listProgramFound->setNoProgramFound(list.isEmpty());
    ui->listProgramFound->addItems(list);
}

void SelectProgramPage::slotItemSelectionChanged()
{
    if (ui->listProgramFound->currentItem()) {
        Q_EMIT programSelected(ui->listProgramFound->currentItem()->text());
    }
}

void SelectProgramPage::slotItemDoubleClicked(QListWidgetItem *item)
{
    if (item) {
        Q_EMIT doubleClicked();
    }
}

void SelectProgramPage::disableSelectProgram()
{
    ui->listProgramFound->setEnabled(false);
}

void SelectProgramPage::slotSelectManualSelectionChanged(bool b)
{
    ui->listProgramFound->setEnabled(!b);
    Q_EMIT selectManualSelectionChanged(b);
}
