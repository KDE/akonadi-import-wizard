/*
   SPDX-FileCopyrightText: 2012-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "thunderbirdaddressbook.h"
#include "addressbook/MorkParser.h"

#include "thunderbirdplugin_debug.h"
#include <KContacts/Addressee>
#include <KLocalizedString>
#include <QUrl>

ThunderBirdAddressBook::ThunderBirdAddressBook(const QDir &dir)
    : mDir(dir)
{
}

ThunderBirdAddressBook::~ThunderBirdAddressBook() = default;

void ThunderBirdAddressBook::importAddressBook()
{
    const QString path = mDir.path();
    readAddressBook(path + QLatin1StringView("/impab.mab"));
    const QStringList filesimportab = mDir.entryList(QStringList(QStringLiteral("impab-[0-9]*.map")), QDir::Files, QDir::Name);
    for (const QString &file : filesimportab) {
        readAddressBook(path + QLatin1Char('/') + file);
    }
    readAddressBook(path + QLatin1StringView("/abook.mab"));

    const QStringList files = mDir.entryList(QStringList(QStringLiteral("abook-[0-9]*.map")), QDir::Files, QDir::Name);
    for (const QString &file : files) {
        readAddressBook(path + QLatin1Char('/') + file);
    }
    readAddressBook(path + QLatin1StringView("/history.mab"));

    cleanUp();
}

void ThunderBirdAddressBook::readAddressBook(const QString &filename)
{
    MorkParser mork;
    if (!mork.open(filename)) {
        if (mork.error() == FailedToOpen) {
            addAddressBookImportError(i18n("Contacts file '%1' not found", filename));
        }
        qCDebug(THUNDERBIRDPLUGIN_LOG) << " error during read file " << filename << " Error type " << mork.error();
        return;
    }
    MorkTableMap *tables = mork.getTables(0x80);
    if (tables) {
        MorkTableMap::iterator tableIterEnd(tables->end());
        for (MorkTableMap::iterator tableIter = tables->begin(); tableIter != tableIterEnd; ++tableIter) {
            if (tableIter.key() != 0) {
                MorkRowMap *rows = mork.getRows(0x80, &tableIter.value());
                if (rows) {
                    MorkRowMap::iterator endRow(rows->end());
                    for (MorkRowMap::iterator rowIter = rows->begin(); rowIter != endRow; ++rowIter) {
                        if (rowIter.key() != 0) {
                            KContacts::Addressee contact;
                            MorkCells cells = rowIter.value();
                            MorkCells::iterator endCellIter = cells.end();
                            KContacts::Address homeAddr = KContacts::Address(KContacts::Address::Home);
                            KContacts::Address workAddr = KContacts::Address(KContacts::Address::Work);
                            int birthday = -1;
                            int birthmonth = -1;
                            int birthyear = -1;

                            for (MorkCells::iterator cellsIter = cells.begin(); cellsIter != endCellIter; ++cellsIter) {
                                const QString value = mork.getValue(cellsIter.value());
                                const QString column = mork.getColumn(cellsIter.key());
                                qCDebug(THUNDERBIRDPLUGIN_LOG) << "column :" << column << " value :" << value;
                                if (column == QLatin1StringView("LastModifiedDate")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("RecordKey")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("AddrCharSet")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("LastRecordKey")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("ns:addrbk:db:table:kind:pab")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("ListName")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("ListNickName")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("ListDescription")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("ListTotalAddresses")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("LowercaseListName")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("ns:addrbk:db:table:kind:deleted")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("PhotoType")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("PreferDisplayName")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("PhotoURI")) {
                                    KContacts::Picture photo;
                                    photo.setUrl(value);
                                    contact.setLogo(photo);
                                } else if (column == QLatin1StringView("PhotoName")) {
                                    // TODO: verify it
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("DbRowID")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("ns:addrbk:db:row:scope:card:all")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("ns:addrbk:db:row:scope:list:all")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("ns:addrbk:db:row:scope:data:all")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("FirstName")) {
                                    contact.setName(value);
                                } else if (column == QLatin1StringView("LastName")) {
                                    contact.setFamilyName(value);
                                } else if (column == QLatin1StringView("PhoneticFirstName")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("PhoneticLastName")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("DisplayName")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("NickName")) {
                                    contact.setNickName(value);
                                } else if (column == QLatin1StringView("PrimaryEmail")) {
                                    contact.setEmails(QStringList() << value);
                                } else if (column == QLatin1StringView("LowercasePrimaryEmail")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("SecondEmail")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("PreferMailFormat")) {
                                    contact.insertCustom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("MailPreferedFormatting"), value);
                                } else if (column == QLatin1StringView("PopularityIndex")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("AllowRemoteContent")) {
                                    contact.insertCustom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("MailAllowToRemoteContent"), value);
                                } else if (column == QLatin1StringView("WorkPhone")) {
                                    contact.insertPhoneNumber(KContacts::PhoneNumber(value, KContacts::PhoneNumber::Work));
                                } else if (column == QLatin1StringView("HomePhone")) {
                                    contact.insertPhoneNumber(KContacts::PhoneNumber(value, KContacts::PhoneNumber::Home));
                                } else if (column == QLatin1StringView("FaxNumber")) {
                                    contact.insertPhoneNumber(KContacts::PhoneNumber(value, KContacts::PhoneNumber::Fax));
                                } else if (column == QLatin1StringView("PagerNumber")) {
                                    contact.insertPhoneNumber(KContacts::PhoneNumber(value, KContacts::PhoneNumber::Pager));
                                } else if (column == QLatin1StringView("CellularNumber")) {
                                    contact.insertPhoneNumber(KContacts::PhoneNumber(value, KContacts::PhoneNumber::Cell));
                                } else if (column == QLatin1StringView("WorkPhoneType")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("HomePhoneType")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("FaxNumberType")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("PagerNumberType")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("CellularNumberType")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("HomeAddress")) {
                                    homeAddr.setStreet(value);
                                } else if (column == QLatin1StringView("HomeAddress2")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("HomeCity")) {
                                    homeAddr.setLocality(value);
                                } else if (column == QLatin1StringView("HomeState")) {
                                    homeAddr.setRegion(value);
                                } else if (column == QLatin1StringView("HomeZipCode")) {
                                    homeAddr.setPostalCode(value);
                                } else if (column == QLatin1StringView("HomeCountry")) {
                                    homeAddr.setCountry(value);
                                } else if (column == QLatin1StringView("WorkAddress")) {
                                    workAddr.setStreet(value);
                                } else if (column == QLatin1StringView("WorkAddress2")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("WorkCity")) {
                                    workAddr.setLocality(value);
                                } else if (column == QLatin1StringView("WorkState")) {
                                    workAddr.setRegion(value);
                                } else if (column == QLatin1StringView("WorkZipCode")) {
                                    workAddr.setPostalCode(value);
                                } else if (column == QLatin1StringView("WorkCountry")) {
                                    workAddr.setCountry(value);
                                } else if (column == QLatin1StringView("JobTitle")) {
                                    contact.setTitle(value);
                                } else if (column == QLatin1StringView("Department")) {
                                    contact.setDepartment(value);
                                } else if (column == QLatin1StringView("Company")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("_AimScreenName")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("AnniversaryYear")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("AnniversaryMonth")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("AnniversaryDay")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("SpouseName")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("FamilyName")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("WebPage1")) {
                                    KContacts::ResourceLocatorUrl url;
                                    url.setUrl(QUrl(value));
                                    contact.setUrl(url);
                                } else if (column == QLatin1StringView("WebPage2")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1StringView("BirthYear")) {
                                    birthyear = value.toInt();
                                } else if (column == QLatin1StringView("BirthMonth")) {
                                    birthmonth = value.toInt();
                                } else if (column == QLatin1StringView("BirthDay")) {
                                    birthday = value.toInt();
                                } else if (column == QLatin1StringView("Custom1")) {
                                    contact.insertCustom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("Custom1"), value);
                                } else if (column == QLatin1StringView("Custom2")) {
                                    contact.insertCustom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("Custom2"), value);
                                } else if (column == QLatin1StringView("Custom3")) {
                                    contact.insertCustom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("Custom3"), value);
                                } else if (column == QLatin1StringView("Custom4")) {
                                    contact.insertCustom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("Custom4"), value);
                                } else if (column == QLatin1StringView("Notes")) {
                                    contact.setNote(value);
                                } else {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " Columnn not implemented " << column;
                                }
                                // qCDebug(THUNDERBIRDPLUGIN_LOG)<<" value :"<<value<<" column"<<column;
                            }

                            if (!homeAddr.isEmpty()) {
                                contact.insertAddress(homeAddr);
                            }
                            if (!workAddr.isEmpty()) {
                                contact.insertAddress(workAddr);
                            }

                            const QDate birthDate(birthyear, birthmonth, birthday);
                            if (birthDate.isValid()) {
                                contact.setBirthday(birthDate);
                            }
                            addImportContactNote(contact, QStringLiteral("Thunderbird"));
                            createContact(contact);
                            qCDebug(THUNDERBIRDPLUGIN_LOG) << "-----------------------";
                        }
                    }
                }
            }
        }
    }
}

#include "moc_thunderbirdaddressbook.cpp"
