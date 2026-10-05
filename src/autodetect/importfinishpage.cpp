/*
   SPDX-FileCopyrightText: 2012-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "importfinishpage.h"

#include "ui_importfinishpage.h"
#include <QTime>

using namespace Qt::Literals::StringLiterals;

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
    ui->logFinish->addInfoLogEntry(u"[%1] "_s.arg(QTime::currentTime().toString()) + log);
}

void ImportFinishPage::addImportError(const QString &log)
{
    ui->logFinish->addErrorLogEntry(log);
}

#include "moc_importfinishpage.cpp"
