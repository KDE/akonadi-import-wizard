/*
   SPDX-FileCopyrightText: 2012-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "importsettingpage.h"
#include "ui_importsettingpage.h"

ImportSettingPage::ImportSettingPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ImportSettingPage)
{
    ui->setupUi(this);
    connect(ui->importSettings, &QPushButton::clicked, this, &ImportSettingPage::importSettingsClicked);
}

ImportSettingPage::~ImportSettingPage()
{
    delete ui;
}

void ImportSettingPage::addImportInfo(const QString &log)
{
    ui->logSettings->addInfoLogEntry(log);
}

void ImportSettingPage::addImportError(const QString &log)
{
    ui->logSettings->addErrorLogEntry(log);
}

void ImportSettingPage::setImportButtonEnabled(bool enabled)
{
    ui->importSettings->setEnabled(enabled);
}
