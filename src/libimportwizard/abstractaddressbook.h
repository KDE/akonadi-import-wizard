/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "abstractbase.h"
#include "libimportwizard_export.h"
#include <AkonadiCore/Collection>

namespace KContacts
{
class Addressee;
class ContactGroup;
}

class KJob;
namespace LibImportWizard
{
class LIBIMPORTWIZARD_EXPORT AbstractAddressBook : public LibImportWizard::AbstractBase
{
    Q_OBJECT
public:
    AbstractAddressBook();
    ~AbstractAddressBook() override;

protected:
    void createGroup(const KContacts::ContactGroup &group);
    void createContact(const KContacts::Addressee &address);

    void addImportInfo(const QString &log) override;
    void addImportError(const QString &log) override;

    void addAddressBookImportInfo(const QString &log);
    void addAddressBookImportError(const QString &log);
    void cleanUp();
    void addImportContactNote(KContacts::Addressee &address, const QString &applicationName);

private:
    void slotStoreDone(KJob *job);
    bool selectAddressBook();
    Akonadi::Collection mCollection;
};
}
