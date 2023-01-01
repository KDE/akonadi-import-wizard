/*
   SPDX-FileCopyrightText: 2012-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "abstractaddressbook.h"

class EvolutionAddressBook : public LibImportWizard::AbstractAddressBook
{
    Q_OBJECT
public:
    EvolutionAddressBook();
    ~EvolutionAddressBook() override;
    void exportEvolutionAddressBook();
};
