/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef SYLPHEEDADDRESSBOOK_H
#define SYLPHEEDADDRESSBOOK_H

#include <ImportWizard/AbstractAddressBook>
#include <QHash>
#include <QDir>
class QString;

class SylpheedAddressBook : public LibImportWizard::AbstractAddressBook
{
    Q_OBJECT
public:
    explicit SylpheedAddressBook(const QDir &dir);
    ~SylpheedAddressBook();

    void importAddressBook();
private:
    void readAddressBook(const QString &filename);
    QHash<QString, QStringList > mAddressBookUid;
    QDir mDir;
};

#endif /* SYLPHEEDADDRESSBOOK_H */
