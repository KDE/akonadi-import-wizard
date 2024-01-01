/*
   SPDX-FileCopyrightText: 2012-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "importfinishpage.h"
#include "ui_importfinishpage.h"
#include <QTime>

ImportFinishPage::ImportFinishPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ImportFinishPage)
{
    ui->setupUi(this);
}

ImportFinishPage::~ImportFinishPage()
{
    delete ui;
}

void ImportFinishPage::addImportInfo(const QString &log)
{
    ui->logFinish->addInfoLogEntry(QStringLiteral("[%1] ").arg(QTime::currentTime().toString()) + log);
}

void ImportFinishPage::addImportError(const QString &log)
{
    ui->logFinish->addErrorLogEntry(log);
}

#include "moc_importfinishpage.cpp"
