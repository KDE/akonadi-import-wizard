/*
   SPDX-FileCopyrightText: 2003 Laurence Anderson <l.d.anderson@warwick.ac.uk>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "manualimportmailpage.h"

#include <QStandardPaths>

ManualImportMailPage::ManualImportMailPage(QWidget *parent)
    : QWidget(parent)
{
    mWidget = new Ui::ManualImportMailPage;
    mWidget->setupUi(this);
    mWidget->mIntroSidebar->setPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("importwizard/pics/step1.png")));
}

ManualImportMailPage::~ManualImportMailPage()
{
    delete mWidget;
}

Ui::ManualImportMailPage *ManualImportMailPage::widget() const
{
    return mWidget;
}
