/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

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

