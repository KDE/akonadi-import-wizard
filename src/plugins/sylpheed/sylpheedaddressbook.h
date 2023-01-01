/*
   SPDX-FileCopyrightText: 2012-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "abstractaddressbook.h"
#include <QDir>
#include <QHash>
class QString;

class SylpheedAddressBook : public LibImportWizard::AbstractAddressBook
{
    Q_OBJECT
public:
    explicit SylpheedAddressBook(const QDir &dir);
    ~SylpheedAddressBook() override;

    void importAddressBook();

private:
    void readAddressBook(const QString &filename);
    QHash<QString, QStringList> mAddressBookUid;
    QDir mDir;
};
