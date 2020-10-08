/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef BalsaAddressBook_H
#define BalsaAddressBook_H

#include "abstractaddressbook.h"

class KConfigGroup;

class BalsaAddressBook : public LibImportWizard::AbstractAddressBook
{
public:
    explicit BalsaAddressBook(const QString &filename);
    ~BalsaAddressBook();

    void importAddressBook();
private:
    void readAddressBook(const KConfigGroup &grp);
    QString mFileName;
};

#endif // BalsaAddressBook_H
