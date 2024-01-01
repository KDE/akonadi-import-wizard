/*
   SPDX-FileCopyrightText: 2012-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "abstractaddressbook.h"
#include <QDir>

class ThunderBirdAddressBook : public LibImportWizard::AbstractAddressBook
{
    Q_OBJECT
public:
    explicit ThunderBirdAddressBook(const QDir &dir);
    ~ThunderBirdAddressBook() override;

    void importAddressBook();

private:
    void readAddressBook(const QString &filename);
    QDir mDir;
};
