/*
   SPDX-FileCopyrightText: 2012-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QWidget>

namespace Ui
{
class ImportMailPage;
}

namespace Akonadi
{
class Collection;
}

namespace MailImporter
{
class ImportMailsWidget;
}

class ImportMailPage : public QWidget
{
    Q_OBJECT
public:
    explicit ImportMailPage(QWidget *parent = nullptr);
    ~ImportMailPage() override;
    MailImporter::ImportMailsWidget *mailWidget();
    Akonadi::Collection selectedCollection() const;
    void setImportButtonEnabled(bool enabled);

Q_SIGNALS:
    void importMailsClicked();

private:
    void collectionChanged(const Akonadi::Collection &collection);
    Ui::ImportMailPage *const ui;
};
