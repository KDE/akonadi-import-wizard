/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "abstractaddressbook.h"
#include "abstractdisplayinfo.h"

#include "libimportwizard_debug.h"
#include <Akonadi/CollectionDialog>
#include <Akonadi/Item>
#include <Akonadi/ItemCreateJob>
#include <KContacts/Addressee>
#include <KContacts/ContactGroup>
#include <KLocalizedString>

#include <QPointer>
using namespace LibImportWizard;
AbstractAddressBook::AbstractAddressBook()
    : mCollection(-1)
{
}

AbstractAddressBook::~AbstractAddressBook() = default;

bool AbstractAddressBook::selectAddressBook()
{
    addAddressBookImportInfo(i18n("Creating new contact..."));
    if (!mCollection.isValid()) {
        const QStringList mimeTypes(KContacts::Addressee::mimeType());
        QPointer<Akonadi::CollectionDialog> dlg = new Akonadi::CollectionDialog(mAbstractDisplayInfo ? mAbstractDisplayInfo->parentWidget() : nullptr);
        dlg->setMimeTypeFilter(mimeTypes);
        dlg->setAccessRightsFilter(Akonadi::Collection::CanCreateItem);
        dlg->setWindowTitle(i18nc("@title:window", "Select Address Book"));
        dlg->setDescription(i18n("Select the address book the new contact shall be saved in:"));

        if (dlg->exec() == QDialog::Accepted && dlg) {
            mCollection = dlg->selectedCollection();
        } else {
            addAddressBookImportError(i18n("Address Book was not selected."));
            delete dlg;
            return false;
        }
        delete dlg;
    }
    return true;
}

void AbstractAddressBook::createGroup(const KContacts::ContactGroup &group)
{
    if (selectAddressBook()) {
        Akonadi::Item item;
        item.setPayload<KContacts::ContactGroup>(group);
        item.setMimeType(KContacts::ContactGroup::mimeType());

        auto job = new Akonadi::ItemCreateJob(item, mCollection);
        connect(job, &Akonadi::ItemCreateJob::result, this, &AbstractAddressBook::slotStoreDone);
    }
}

void AbstractAddressBook::addImportContactNote(KContacts::Addressee &address, const QString &applicationName)
{
    QString currentNote = address.note();
    if (!currentNote.isEmpty()) {
        currentNote += QLatin1Char('\n');
    }
    currentNote += i18n("Imported from \"%1\"", applicationName);
    address.setNote(currentNote);
}

void AbstractAddressBook::createContact(const KContacts::Addressee &address)
{
    if (selectAddressBook()) {
        Akonadi::Item item;
        item.setPayload<KContacts::Addressee>(address);
        item.setMimeType(KContacts::Addressee::mimeType());
        auto job = new Akonadi::ItemCreateJob(item, mCollection);
        connect(job, &Akonadi::ItemCreateJob::result, this, &AbstractAddressBook::slotStoreDone);
    }
}

void AbstractAddressBook::slotStoreDone(KJob *job)
{
    if (job->error()) {
        qCDebug(LIBIMPORTWIZARD_LOG) << " job->errorString() : " << job->errorString();
        addAddressBookImportError(i18n("Error during contact creation: %1", job->errorString()));
        return;
    }
    addAddressBookImportInfo(i18n("Contact creation complete"));
}

void AbstractAddressBook::addImportInfo(const QString &log)
{
    addAddressBookImportInfo(log);
}

void AbstractAddressBook::addImportError(const QString &log)
{
    addAddressBookImportError(log);
}

void AbstractAddressBook::addAddressBookImportInfo(const QString &log)
{
    if (mAbstractDisplayInfo) {
        mAbstractDisplayInfo->addressbookImportInfo(log);
    }
}

void AbstractAddressBook::addAddressBookImportError(const QString &log)
{
    if (mAbstractDisplayInfo) {
        mAbstractDisplayInfo->addressbookImportError(log);
    }
}

void AbstractAddressBook::cleanUp()
{
    mCollection = Akonadi::Collection();
}
