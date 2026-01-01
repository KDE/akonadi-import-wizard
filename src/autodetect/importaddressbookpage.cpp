/*
   SPDX-FileCopyrightText: 2012-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "importaddressbookpage.h"
#include "ui_importaddressbookpage.h"

ImportAddressbookPage::ImportAddressbookPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ImportAddressbookPage)
{
    ui->setupUi(this);
    connect(ui->importAddressBook, &QPushButton::clicked, this, &ImportAddressbookPage::importAddressbookClicked);
}

ImportAddressbookPage::~ImportAddressbookPage()
{
    delete ui;
}

void ImportAddressbookPage::addImportInfo(const QString &log)
{
    ui->logAddressbook->addInfoLogEntry(log);
}

void ImportAddressbookPage::addImportError(const QString &log)
{
    ui->logAddressbook->addErrorLogEntry(log);
}

void ImportAddressbookPage::setImportButtonEnabled(bool enabled)
{
    ui->importAddressBook->setEnabled(enabled);
}

#include "moc_importaddressbookpage.cpp"
