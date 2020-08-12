/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "importfilterpage.h"
#include "ui_importfilterpage.h"

ImportFilterPage::ImportFilterPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ImportFilterPage)
{
    ui->setupUi(this);
    connect(ui->importFilters, &QPushButton::clicked, this, &ImportFilterPage::importFiltersClicked);
}

ImportFilterPage::~ImportFilterPage()
{
    delete ui;
}

void ImportFilterPage::addImportInfo(const QString &log)
{
    ui->logFilters->addInfoLogEntry(log);
}

void ImportFilterPage::addImportError(const QString &log)
{
    ui->logFilters->addErrorLogEntry(log);
}

void ImportFilterPage::setImportButtonEnabled(bool enabled)
{
    ui->importFilters->setEnabled(enabled);
}
