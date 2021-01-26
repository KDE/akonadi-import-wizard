/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef THUNDERBIRDADDRESSBOOK_H
#define THUNDERBIRDADDRESSBOOK_H

#include "abstractaddressbook.h"
#include <QDir>

class ThunderBirdAddressBook : public LibImportWizard::AbstractAddressBook
{
    Q_OBJECT
public:
    explicit ThunderBirdAddressBook(const QDir &dir);
    ~ThunderBirdAddressBook();

    void importAddressBook();

private:
    void readAddressBook(const QString &filename);
    QDir mDir;
};

#endif // THUNDERBIRDADDRESSBOOK_H
