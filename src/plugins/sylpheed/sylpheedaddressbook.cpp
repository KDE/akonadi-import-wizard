/*
   SPDX-FileCopyrightText: 2012-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "sylpheedaddressbook.h"
using namespace Qt::Literals::StringLiterals;

#include <KContacts/Addressee>
#include <KContacts/ContactGroup>

#include "sylpheedplugin_debug.h"
#include <KLocalizedString>

#include <QDomDocument>

SylpheedAddressBook::SylpheedAddressBook(const QDir &dir)
    : mDir(dir)
{
}

SylpheedAddressBook::~SylpheedAddressBook() = default;

void SylpheedAddressBook::importAddressBook()
{
    const QStringList files = mDir.entryList(QStringList(u"addrbook-[0-9]*.xml"_s), QDir::Files, QDir::Name);
    if (files.isEmpty()) {
        addAddressBookImportInfo(i18n("No addressbook found"));
    } else {
        for (const QString &file : files) {
            readAddressBook(mDir.path() + u'/' + file);
        }
    }
    cleanUp();
}

void SylpheedAddressBook::readAddressBook(const QString &filename)
{
    QFile file(filename);
    // qCDebug(SYLPHEEDPLUGIN_LOG)<<" import filename :"<<filename;
    if (!file.open(QIODevice::ReadOnly)) {
        qCDebug(SYLPHEEDPLUGIN_LOG) << " We can't open file" << filename;
        return;
    }
    QDomDocument doc;
    const QDomDocument::ParseResult parseResult = doc.setContent(&file);
    if (!parseResult) {
        qCDebug(SYLPHEEDPLUGIN_LOG) << "Unable to load document.Parse error in line " << parseResult.errorLine << ", col " << parseResult.errorColumn << ": "
                                    << qPrintable(parseResult.errorMessage);
        return;
    }
    QDomElement domElement = doc.documentElement();

    if (domElement.isNull()) {
        qCDebug(SYLPHEEDPLUGIN_LOG) << "addressbook not found";
        return;
    }

    QDomElement e = domElement.firstChildElement();
    if (e.isNull()) {
        addAddressBookImportError(i18n("No contact found in %1", filename));
        return;
    }

    for (; !e.isNull(); e = e.nextSiblingElement()) {
        // TODO ?? const QString name = e.attribute(u"name"_s);

        const QString tag = e.tagName();
        if (tag == QLatin1StringView("person")) {
            KContacts::Addressee contact;
            // uid="333304265" first-name="dd" last-name="ccc" nick-name="" cn="laurent"
            QString uidPerson;
            if (e.hasAttribute(u"uid"_s)) {
                uidPerson = e.attribute(u"uid"_s);
            }
            if (e.hasAttribute(u"first-name"_s)) {
                contact.setName(e.attribute(u"first-name"_s));
            }
            if (e.hasAttribute(u"last-name"_s)) {
                contact.setFamilyName(e.attribute(u"last-name"_s));
            }
            if (e.hasAttribute(u"nick-name"_s)) {
                contact.setNickName(e.attribute(u"nick-name"_s));
            }
            if (e.hasAttribute(u"cn"_s)) {
                contact.setFormattedName(e.attribute(u"cn"_s));
            }
            QStringList uidAddress;
            for (QDomElement addressElement = e.firstChildElement(); !addressElement.isNull(); addressElement = addressElement.nextSiblingElement()) {
                const QString addressTag = addressElement.tagName();
                if (addressTag == QLatin1StringView("address-list")) {
                    QStringList emails;
                    for (QDomElement addresslist = addressElement.firstChildElement(); !addresslist.isNull(); addresslist = addresslist.nextSiblingElement()) {
                        const QString tagAddressList = addresslist.tagName();
                        if (tagAddressList == QLatin1StringView("address")) {
                            if (addresslist.hasAttribute(u"email"_s)) {
                                emails << addresslist.attribute(u"email"_s);
                            } else if (addresslist.hasAttribute(u"alias"_s)) {
                                // TODO:
                            } else if (addresslist.hasAttribute(u"uid"_s)) {
                                uidAddress << addresslist.attribute(u"uid"_s);
                            }
                        } else {
                            qCDebug(SYLPHEEDPLUGIN_LOG) << " tagAddressList unknown :" << tagAddressList;
                        }
                    }
                    if (!emails.isEmpty()) {
                        contact.setEmails(emails);
                    }
                } else if (addressTag == QLatin1StringView("attribute-list")) {
                    for (QDomElement attributelist = addressElement.firstChildElement(); !attributelist.isNull();
                         attributelist = attributelist.nextSiblingElement()) {
                        const QString tagAttributeList = attributelist.tagName();
                        if (tagAttributeList == QLatin1StringView("attribute")) {
                            if (attributelist.hasAttribute(u"name"_s)) {
                                const QString name = attributelist.attribute(u"name"_s);
                                const QString value = attributelist.text();
                                contact.insertCustom(u"KADDRESSBOOK"_s, name, value);
                            }
                        } else {
                            qCDebug(SYLPHEEDPLUGIN_LOG) << "tagAttributeList not implemented " << tagAttributeList;
                        }
                    }
                } else {
                    qCDebug(SYLPHEEDPLUGIN_LOG) << " addressTag unknown :" << addressTag;
                }
            }
            if (!mAddressBookUid.contains(uidPerson)) {
                mAddressBookUid.insert(uidPerson, uidAddress);
            } else {
                qCDebug(SYLPHEEDPLUGIN_LOG) << " problem uidPerson already stored" << uidPerson;
            }
            addImportContactNote(contact, u"Sylpheed"_s);
            createContact(contact);
        } else if (tag == QLatin1StringView("group")) {
            QString name = e.attribute(u"name"_s);
            KContacts::ContactGroup group(name);
            // TODO: create Group
            for (QDomElement groupElement = e.firstChildElement(); !groupElement.isNull(); groupElement = groupElement.nextSiblingElement()) {
                const QString groupTag = groupElement.tagName();
                if (groupTag == QLatin1StringView("member-list")) {
                    for (QDomElement memberlist = groupElement.firstChildElement(); !memberlist.isNull(); memberlist = memberlist.nextSiblingElement()) {
                        const QString tagMemberList = memberlist.tagName();
                        if (tagMemberList == QLatin1StringView("member")) {
                            QString pid;
                            QString eid;
                            if (memberlist.hasAttribute(u"pid"_s)) {
                                pid = memberlist.attribute(u"pid"_s);
                            }
                            if (memberlist.hasAttribute(u"eid"_s)) {
                                eid = memberlist.attribute(u"eid"_s);
                            }
                            if (!pid.isEmpty() && !eid.isEmpty()) {
                                // TODO
                            } else {
                                qCDebug(SYLPHEEDPLUGIN_LOG) << " Problem with group" << name;
                            }
                            // TODO
                        }
                    }
                }
            }
            createGroup(group);
        } else {
            qCDebug(SYLPHEEDPLUGIN_LOG) << " SylpheedAddressBook::readAddressBook  tag unknown :" << tag;
        }
    }
}

#include "moc_sylpheedaddressbook.cpp"
