/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "importmailpage.h"
#include "ui_importmailpage.h"

ImportMailPage::ImportMailPage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ImportMailPage)
{
    ui->setupUi(this);
    connect(ui->importMails, &QPushButton::clicked, this, &ImportMailPage::importMailsClicked);
    connect(ui->mCollectionRequestor, &MailCommon::FolderRequester::folderChanged, this, &ImportMailPage::collectionChanged);
}

ImportMailPage::~ImportMailPage()
{
    delete ui;
}

MailImporter::ImportMailsWidget *ImportMailPage::mailWidget()
{
    return ui->mMailImporterWidget;
}

void ImportMailPage::collectionChanged(const Akonadi::Collection &collection)
{
    ui->importMails->setEnabled(collection.isValid());
}

Akonadi::Collection ImportMailPage::selectedCollection() const
{
    return ui->mCollectionRequestor->collection();
}

void ImportMailPage::setImportButtonEnabled(bool enabled)
{
    ui->importMails->setEnabled(enabled);
}
