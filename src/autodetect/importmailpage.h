/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef IMPORTMAILPAGE_H
#define IMPORTMAILPAGE_H

#include <QWidget>

namespace Ui {
class ImportMailPage;
}

namespace Akonadi {
class Collection;
}

namespace MailImporter {
class ImportMailsWidget;
}

class ImportMailPage : public QWidget
{
    Q_OBJECT
public:
    explicit ImportMailPage(QWidget *parent = nullptr);
    ~ImportMailPage();
    MailImporter::ImportMailsWidget *mailWidget();
    Akonadi::Collection selectedCollection() const;
    void setImportButtonEnabled(bool enabled);

Q_SIGNALS:
    void importMailsClicked();

private:
    void collectionChanged(const Akonadi::Collection &collection);
    Ui::ImportMailPage *const ui;
};

#endif // IMPORTMAILPAGE_H
