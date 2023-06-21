/*
   SPDX-FileCopyrightText: 2012-2023 Laurent Montel <montel@kde.org>

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
    readAddressBook(path + QLatin1String("/impab.mab"));
    const QStringList filesimportab = mDir.entryList(QStringList(QStringLiteral("impab-[0-9]*.map")), QDir::Files, QDir::Name);
    for (const QString &file : filesimportab) {
        readAddressBook(path + QLatin1Char('/') + file);
    }
    readAddressBook(path + QLatin1String("/abook.mab"));

    const QStringList files = mDir.entryList(QStringList(QStringLiteral("abook-[0-9]*.map")), QDir::Files, QDir::Name);
    for (const QString &file : files) {
        readAddressBook(path + QLatin1Char('/') + file);
    }
    readAddressBook(path + QLatin1String("/history.mab"));

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
                                if (column == QLatin1String("LastModifiedDate")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("RecordKey")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("AddrCharSet")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("LastRecordKey")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("ns:addrbk:db:table:kind:pab")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("ListName")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("ListNickName")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("ListDescription")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("ListTotalAddresses")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("LowercaseListName")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("ns:addrbk:db:table:kind:deleted")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("PhotoType")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("PreferDisplayName")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("PhotoURI")) {
                                    KContacts::Picture photo;
                                    photo.setUrl(value);
                                    contact.setLogo(photo);
                                } else if (column == QLatin1String("PhotoName")) {
                                    // TODO: verify it
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("DbRowID")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("ns:addrbk:db:row:scope:card:all")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("ns:addrbk:db:row:scope:list:all")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("ns:addrbk:db:row:scope:data:all")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("FirstName")) {
                                    contact.setName(value);
                                } else if (column == QLatin1String("LastName")) {
                                    contact.setFamilyName(value);
                                } else if (column == QLatin1String("PhoneticFirstName")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("PhoneticLastName")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("DisplayName")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("NickName")) {
                                    contact.setNickName(value);
                                } else if (column == QLatin1String("PrimaryEmail")) {
                                    contact.setEmails(QStringList() << value);
                                } else if (column == QLatin1String("LowercasePrimaryEmail")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("SecondEmail")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("PreferMailFormat")) {
                                    contact.insertCustom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("MailPreferedFormatting"), value);
                                } else if (column == QLatin1String("PopularityIndex")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("AllowRemoteContent")) {
                                    contact.insertCustom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("MailAllowToRemoteContent"), value);
                                } else if (column == QLatin1String("WorkPhone")) {
                                    contact.insertPhoneNumber(KContacts::PhoneNumber(value, KContacts::PhoneNumber::Work));
                                } else if (column == QLatin1String("HomePhone")) {
                                    contact.insertPhoneNumber(KContacts::PhoneNumber(value, KContacts::PhoneNumber::Home));
                                } else if (column == QLatin1String("FaxNumber")) {
                                    contact.insertPhoneNumber(KContacts::PhoneNumber(value, KContacts::PhoneNumber::Fax));
                                } else if (column == QLatin1String("PagerNumber")) {
                                    contact.insertPhoneNumber(KContacts::PhoneNumber(value, KContacts::PhoneNumber::Pager));
                                } else if (column == QLatin1String("CellularNumber")) {
                                    contact.insertPhoneNumber(KContacts::PhoneNumber(value, KContacts::PhoneNumber::Cell));
                                } else if (column == QLatin1String("WorkPhoneType")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("HomePhoneType")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("FaxNumberType")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("PagerNumberType")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("CellularNumberType")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("HomeAddress")) {
                                    homeAddr.setStreet(value);
                                } else if (column == QLatin1String("HomeAddress2")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("HomeCity")) {
                                    homeAddr.setLocality(value);
                                } else if (column == QLatin1String("HomeState")) {
                                    homeAddr.setRegion(value);
                                } else if (column == QLatin1String("HomeZipCode")) {
                                    homeAddr.setPostalCode(value);
                                } else if (column == QLatin1String("HomeCountry")) {
                                    homeAddr.setCountry(value);
                                } else if (column == QLatin1String("WorkAddress")) {
                                    workAddr.setStreet(value);
                                } else if (column == QLatin1String("WorkAddress2")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("WorkCity")) {
                                    workAddr.setLocality(value);
                                } else if (column == QLatin1String("WorkState")) {
                                    workAddr.setRegion(value);
                                } else if (column == QLatin1String("WorkZipCode")) {
                                    workAddr.setPostalCode(value);
                                } else if (column == QLatin1String("WorkCountry")) {
                                    workAddr.setCountry(value);
                                } else if (column == QLatin1String("JobTitle")) {
                                    contact.setTitle(value);
                                } else if (column == QLatin1String("Department")) {
                                    contact.setDepartment(value);
                                } else if (column == QLatin1String("Company")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("_AimScreenName")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("AnniversaryYear")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("AnniversaryMonth")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("AnniversaryDay")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("SpouseName")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("FamilyName")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("WebPage1")) {
                                    KContacts::ResourceLocatorUrl url;
                                    url.setUrl(QUrl(value));
                                    contact.setUrl(url);
                                } else if (column == QLatin1String("WebPage2")) {
                                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " column " << column << " found but not imported. Need to look at how to import it";
                                } else if (column == QLatin1String("BirthYear")) {
                                    birthyear = value.toInt();
                                } else if (column == QLatin1String("BirthMonth")) {
                                    birthmonth = value.toInt();
                                } else if (column == QLatin1String("BirthDay")) {
                                    birthday = value.toInt();
                                } else if (column == QLatin1String("Custom1")) {
                                    contact.insertCustom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("Custom1"), value);
                                } else if (column == QLatin1String("Custom2")) {
                                    contact.insertCustom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("Custom2"), value);
                                } else if (column == QLatin1String("Custom3")) {
                                    contact.insertCustom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("Custom3"), value);
                                } else if (column == QLatin1String("Custom4")) {
                                    contact.insertCustom(QStringLiteral("KADDRESSBOOK"), QStringLiteral("Custom4"), value);
                                } else if (column == QLatin1String("Notes")) {
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
