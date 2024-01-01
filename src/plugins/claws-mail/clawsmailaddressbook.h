/*
   SPDX-FileCopyrightText: 2012-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "../sylpheed/sylpheedaddressbook.h"
#include <QDir>

class ClawsMailAddressBook : public SylpheedAddressBook
{
public:
    explicit ClawsMailAddressBook(const QDir &dir);
    ~ClawsMailAddressBook() override;
};
