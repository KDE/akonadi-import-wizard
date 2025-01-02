/*
   SPDX-FileCopyrightText: 2012-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "clawsmailaddressbook.h"
#include <ImportWizard/ImportWizardUtil>

ClawsMailAddressBook::ClawsMailAddressBook(const QDir &dir)
    : SylpheedAddressBook(dir)
{
}

ClawsMailAddressBook::~ClawsMailAddressBook() = default;
