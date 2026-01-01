/*
   SPDX-FileCopyrightText: 2012-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "trojitaaddressbook.h"
using namespace Qt::Literals::StringLiterals;

#include <KContacts/Addressee>

#include <QUrl>

#include <QSettings>

TrojitaAddressBook::TrojitaAddressBook(const QString &filename)
{
    settings = new QSettings(filename, QSettings::IniFormat, this);
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
        contactABC.setEmails(QStringList() << settings->value(u"email"_s).toStringList());

        KContacts::Address homeAddr = KContacts::Address(KContacts::Address::Home);
        homeAddr.setLocality(settings->value(u"city"_s).toString());
        homeAddr.setRegion(settings->value(u"state"_s).toString());
        homeAddr.setPostalCode(settings->value(u"zip"_s).toString());
        homeAddr.setCountry(settings->value(u"country"_s).toString());
        homeAddr.setStreet(settings->value(u"address"_s).toString());
        if (!homeAddr.isEmpty()) {
            contactABC.insertAddress(homeAddr);
        }

        contactABC.insertPhoneNumber(KContacts::PhoneNumber(settings->value(u"phone"_s).toString(), KContacts::PhoneNumber::Home));
        contactABC.insertPhoneNumber(KContacts::PhoneNumber(settings->value(u"workphone"_s).toString(), KContacts::PhoneNumber::Work));
        contactABC.insertPhoneNumber(KContacts::PhoneNumber(settings->value(u"fax"_s).toString(), KContacts::PhoneNumber::Fax));
        contactABC.insertPhoneNumber(KContacts::PhoneNumber(settings->value(u"mobile"_s).toString(), KContacts::PhoneNumber::Cell));
        contactABC.setNickName(settings->value(u"nick"_s).toString());
        KContacts::ResourceLocatorUrl url;
        url.setUrl(QUrl(settings->value(u"url"_s).toString()));
        contactABC.setUrl(url);

        const QDate birthDate = QDate::fromString(settings->value(u"anniversary"_s).toString());
        if (birthDate.isValid()) {
            contactABC.setBirthday(birthDate);
        }
        // TODO
        // ADD(Photo, "photo");
        addImportContactNote(contactABC, u"Trojita"_s);
        createContact(contactABC);
        settings->endGroup();
    }
}
