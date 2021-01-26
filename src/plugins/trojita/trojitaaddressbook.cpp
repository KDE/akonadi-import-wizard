/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "trojitaaddressbook.h"
#include <KContacts/Addressee>

#include <QUrl>

#include <QSettings>

TrojitaAddressBook::TrojitaAddressBook(const QString &filename)
{
    settings = new QSettings(filename, QSettings::IniFormat, this);
    settings->setIniCodec("UTF-8");
}

TrojitaAddressBook::~TrojitaAddressBook()
{
    delete settings;
}

void TrojitaAddressBook::readAddressBook()
{
    const QStringList contacts = settings->childGroups();
    for (const QString &contact : contacts) {
        KContacts::Addressee contactABC;
        settings->beginGroup(contact);
        contactABC.setEmails(QStringList() << settings->value(QStringLiteral("email")).toStringList());

        KContacts::Address homeAddr = KContacts::Address(KContacts::Address::Home);
        homeAddr.setLocality(settings->value(QStringLiteral("city")).toString());
        homeAddr.setRegion(settings->value(QStringLiteral("state")).toString());
        homeAddr.setPostalCode(settings->value(QStringLiteral("zip")).toString());
        homeAddr.setCountry(settings->value(QStringLiteral("country")).toString());
        homeAddr.setStreet(settings->value(QStringLiteral("address")).toString());
        if (!homeAddr.isEmpty()) {
            contactABC.insertAddress(homeAddr);
        }

        contactABC.insertPhoneNumber(KContacts::PhoneNumber(settings->value(QStringLiteral("phone")).toString(), KContacts::PhoneNumber::Home));
        contactABC.insertPhoneNumber(KContacts::PhoneNumber(settings->value(QStringLiteral("workphone")).toString(), KContacts::PhoneNumber::Work));
        contactABC.insertPhoneNumber(KContacts::PhoneNumber(settings->value(QStringLiteral("fax")).toString(), KContacts::PhoneNumber::Fax));
        contactABC.insertPhoneNumber(KContacts::PhoneNumber(settings->value(QStringLiteral("mobile")).toString(), KContacts::PhoneNumber::Cell));
        contactABC.setNickName(settings->value(QStringLiteral("nick")).toString());
        KContacts::ResourceLocatorUrl url;
        url.setUrl(QUrl(settings->value(QStringLiteral("url")).toString()));
        contactABC.setUrl(url);

        const QDate birthDate = QDate::fromString(settings->value(QStringLiteral("anniversary")).toString());
        if (birthDate.isValid()) {
            contactABC.setBirthday(birthDate);
        }
        // TODO
        // ADD(Photo, "photo");
        addImportContactNote(contactABC, QStringLiteral("Trojita"));
        createContact(contactABC);
        settings->endGroup();
    }
}
