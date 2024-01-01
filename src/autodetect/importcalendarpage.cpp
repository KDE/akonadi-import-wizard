/*
   SPDX-FileCopyrightText: 2012-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "importcalendarpage.h"
#include "ui_importcalendarpage.h"

ImportCalendarPage::ImportCalendarPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ImportCalendarPage)
{
    ui->setupUi(this);
    connect(ui->importCalendar, &QPushButton::clicked, this, &ImportCalendarPage::importCalendarClicked);
}

ImportCalendarPage::~ImportCalendarPage()
{
    delete ui;
}

void ImportCalendarPage::addImportInfo(const QString &log)
{
    ui->logCalendar->addInfoLogEntry(log);
}

void ImportCalendarPage::addImportError(const QString &log)
{
    ui->logCalendar->addErrorLogEntry(log);
}

void ImportCalendarPage::setImportButtonEnabled(bool enabled)
{
    ui->importCalendar->setEnabled(enabled);
}

#include "moc_importcalendarpage.cpp"
