/*
   SPDX-FileCopyrightText: 2012-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "abstractaddressbook.h"
#include <QHash>
class QSettings;

class TrojitaAddressBook : public LibImportWizard::AbstractAddressBook
{
public:
    explicit TrojitaAddressBook(const QString &filename);
    ~TrojitaAddressBook() override;

    void readAddressBook();

private:
    QSettings *settings = nullptr;
};

