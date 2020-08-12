/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef TrojitaAddressBook_H
#define TrojitaAddressBook_H

#include <ImportWizard/AbstractAddressBook>
#include <QHash>
class QSettings;

class TrojitaAddressBook : public LibImportWizard::AbstractAddressBook
{
public:
    explicit TrojitaAddressBook(const QString &filename);
    ~TrojitaAddressBook();

    void readAddressBook();

private:
    QSettings *settings = nullptr;
};

#endif /* TrojitaAddressBook_H */
