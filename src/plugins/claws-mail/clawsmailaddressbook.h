/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ClawsMailAddressBook_H
#define ClawsMailAddressBook_H

#include "../sylpheed/sylpheedaddressbook.h"
#include <QDir>

class ClawsMailAddressBook : public SylpheedAddressBook
{
public:
    explicit ClawsMailAddressBook(const QDir &dir);
    ~ClawsMailAddressBook();
};

#endif // ClawsMailAddressBook_H
