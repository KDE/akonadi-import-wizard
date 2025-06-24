/*
   SPDX-FileCopyrightText: 2012-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "balsaaddressbook.h"
using namespace Qt::Literals::StringLiterals;

#include "importwizardutil.h"

#include <KContacts/Addressee>
#include <KContacts/LDIFConverter>

#include <KConfig>
#include <KConfigGroup>
#include <KLocalizedString>
#include <QUrl>

#include "balsaplugin_debug.h"
#include <QFile>
#include <QFileInfo>
#include <QRegularExpression>

BalsaAddressBook::BalsaAddressBook(const QString &filename)
    : mFileName(filename)
{
}

BalsaAddressBook::~BalsaAddressBook() = default;

void BalsaAddressBook::importAddressBook()
{
    KConfig config(mFileName);
    const QStringList addressBookList = config.groupList().filter(QRegularExpression(u"address-book-\\d+"_s));
    if (addressBookList.isEmpty()) {
        addAddressBookImportInfo(i18n("No addressbook found"));
    } else {
        for (const QString &addressbook : addressBookList) {
            KConfigGroup grp = config.group(addressbook);
            readAddressBook(grp);
        }
    }
}

void BalsaAddressBook::readAddressBook(const KConfigGroup &grp)
{
    const QString type = grp.readEntry(u"Type"_s);
    if (type.isEmpty()) {
        addAddressBookImportInfo(i18n("No addressbook found"));
        return;
    }
    const QString name = grp.readEntry(u"Name"_s);

    if (type == QLatin1StringView("LibBalsaAddressBookLdap")) {
        ldapStruct ldap;
        ldap.dn = grp.readEntry(u"BaseDN"_s);
        ldap.useTLS = (grp.readEntry(u"EnableTLS"_s) == QLatin1StringView("true"));
        ldap.ldapUrl = QUrl(grp.readEntry(u"Host"_s));
        ldap.port = ldap.ldapUrl.port();
        // TODO: verify
        const QString bookDN = grp.readEntry(u"BookDN"_s); // TODO ?
        ImportWizardUtil::mergeLdap(ldap);
        addAddressBookImportInfo(i18n("Ldap created"));
    } else if (type == QLatin1StringView("LibBalsaAddressBookGpe")) {
        qCDebug(BALSAPLUGIN_LOG) << " Import it !";
    } else if (type == QLatin1StringView("LibBalsaAddressBookLdif")) {
        const QString path = grp.readEntry(u"Path"_s);
        if (!path.isEmpty()) {
            KContacts::Addressee::List contacts;
            KContacts::ContactGroup::List contactsGroup;
            QFile file(path);
            if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QTextStream stream(&file);
                stream.setEncoding(QStringConverter::Encoding::Latin1);

                const QString wholeFile = stream.readAll();
                const QDateTime dtDefault = QFileInfo(file).lastModified();
                file.close();

                KContacts::LDIFConverter::LDIFToAddressee(wholeFile, contacts, contactsGroup, dtDefault);
                for (KContacts::Addressee contact : std::as_const(contacts)) {
                    addImportContactNote(contact, u"Balsa"_s);
                    createContact(contact);
                }
            }
        }
    } else if (type == QLatin1StringView("LibBalsaAddressBookVcard")) {
        const QString path = grp.readEntry(u"Path"_s);
        if (!path.isEmpty()) {
            QMap<QString, QVariant> settings;
            settings.insert(u"Path"_s, path);
            settings.insert(u"DisplayName"_s, name);
            addAddressBookImportInfo(i18n("New addressbook created: %1", createResource(u"akonadi_vcard_resource"_s, name, settings)));
        }
    } else {
        qCDebug(BALSAPLUGIN_LOG) << " unknown addressbook type :" << type;
    }
}
