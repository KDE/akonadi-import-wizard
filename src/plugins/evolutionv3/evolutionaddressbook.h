/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef EVOLUTIONADDRESSBOOK_H
#define EVOLUTIONADDRESSBOOK_H

#include "abstractaddressbook.h"

class EvolutionAddressBook : public LibImportWizard::AbstractAddressBook
{
    Q_OBJECT
public:
    EvolutionAddressBook();
    ~EvolutionAddressBook();
    void exportEvolutionAddressBook();
};

#endif // ABSTRACTADDRESSBOOK_H
