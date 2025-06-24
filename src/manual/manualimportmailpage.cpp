/*
   SPDX-FileCopyrightText: 2003 Laurence Anderson <l.d.anderson@warwick.ac.uk>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "manualimportmailpage.h"
using namespace Qt::Literals::StringLiterals;

#include <QStandardPaths>

ManualImportMailPage::ManualImportMailPage(QWidget *parent)
    : QWidget(parent)
{
    mWidget = new Ui::ManualImportMailPage;
    mWidget->setupUi(this);
    mWidget->mIntroSidebar->setPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation, u"importwizard/pics/step1.png"_s));
}

ManualImportMailPage::~ManualImportMailPage()
{
    delete mWidget;
}

Ui::ManualImportMailPage *ManualImportMailPage::widget() const
{
    return mWidget;
}

#include "moc_manualimportmailpage.cpp"
