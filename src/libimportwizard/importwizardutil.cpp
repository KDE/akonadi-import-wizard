/*
   SPDX-FileCopyrightText: 2012-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "importwizardutil.h"
using namespace Qt::Literals::StringLiterals;

#include "importwizardsavepasswordjob.h"
#include "libimportwizard_debug.h"
#include <Akonadi/Tag>
#include <Akonadi/TagAttribute>
#include <Akonadi/TagCreateJob>
#include <KConfigGroup>
#include <KSharedConfig>

void ImportWizardUtil::mergeLdap(const ldapStruct &ldap)
{
    KSharedConfigPtr ldapConfig = KSharedConfig::openConfig(u"kabldaprc"_s);
    int numberOfLdapSelected = 0;
    KConfigGroup grp;
    if (ldapConfig->hasGroup(u"LDAP"_s)) {
        grp = ldapConfig->group(u"LDAP"_s);
        numberOfLdapSelected = grp.readEntry(u"NumSelectedHosts"_s, 0);
        grp.writeEntry(u"NumSelectedHosts"_s, (numberOfLdapSelected + 1));
    } else {
        grp = ldapConfig->group(u"LDAP"_s);
        grp.writeEntry(u"NumSelectedHosts"_s, 1);

        KConfigGroup ldapSeach = ldapConfig->group(u"LDAPSearch"_s);
        ldapSeach.writeEntry(u"SearchType"_s, 0);
    }
    const int port = ldap.port;
    if (port != -1) {
        grp.writeEntry(u"SelectedPort%1"_s.arg(numberOfLdapSelected), port);
    }
    grp.writeEntry(u"SelectedHost%1"_s.arg(numberOfLdapSelected), ldap.ldapUrl.host());
    if (ldap.useSSL) {
        grp.writeEntry(u"SelectedSecurity%1"_s.arg(numberOfLdapSelected), u"SSL"_s);
    } else if (ldap.useTLS) {
        grp.writeEntry(u"SelectedSecurity%1"_s.arg(numberOfLdapSelected), u"TLS"_s);
    } else {
        grp.writeEntry(u"SelectedSecurity%1"_s.arg(numberOfLdapSelected), u"None"_s);
    }

    if (ldap.saslMech == QLatin1StringView("GSSAPI")) {
        grp.writeEntry(u"SelectedMech%1"_s.arg(numberOfLdapSelected), u"GSSAPI"_s);
        grp.writeEntry(u"SelectedAuth%1"_s.arg(numberOfLdapSelected), u"SASL"_s);
    } else if (ldap.saslMech.isEmpty()) {
        grp.writeEntry(u"SelectedMech%1"_s.arg(numberOfLdapSelected), u"PLAIN"_s);
        grp.writeEntry(u"SelectedAuth%1"_s.arg(numberOfLdapSelected), u"Simple"_s);
    } else {
        qCDebug(LIBIMPORTWIZARD_LOG) << " Mech SASL undefined" << ldap.saslMech;
    }
    grp.writeEntry(u"SelectedVersion%1"_s.arg(numberOfLdapSelected), QString::number(3));
    grp.writeEntry(u"SelectedBind%1"_s.arg(numberOfLdapSelected), ldap.dn);
    // TODO: Verify selectedbase
    grp.writeEntry(u"SelectedBase%1"_s.arg(numberOfLdapSelected), ldap.ldapUrl.path());
    if (ldap.timeout != -1) {
        grp.writeEntry(u"SelectedTimeLimit%1"_s.arg(numberOfLdapSelected), ldap.timeout);
    }
    if (ldap.limit != -1) {
        grp.writeEntry(u"SelectedSizeLimit%1"_s.arg(numberOfLdapSelected), ldap.limit);
    }
    if (!ldap.password.isEmpty()) {
        storePassword(u"SelectedPwdBind%1"_s.arg(numberOfLdapSelected), ImportWizardUtil::Ldap, ldap.password);
    }
    grp.sync();
}

void ImportWizardUtil::addAkonadiTag(const QList<tagStruct> &tagList)
{
    for (int i = 0; i < tagList.size(); ++i) {
        Akonadi::Tag tag(tagList.at(i).name);
        if (tagList.at(i).color.isValid()) {
            tag.attribute<Akonadi::TagAttribute>(Akonadi::Tag::AddIfMissing)->setTextColor(tagList.at(i).color);
        }
        new Akonadi::TagCreateJob(tag);
    }
}

void ImportWizardUtil::storePassword(const QString &name, ImportWizardUtil::ResourceType type, const QString &password)
{
    auto job = new ImportWizardSavePasswordJob;
    switch (type) {
    case Imap:
        job->setName(u"imap"_s);
        job->setPassword(password);
        job->setKey(name + QLatin1StringView("rc"));
        break;
    case Pop3:
        job->setName(u"pop3"_s);
        job->setPassword(password);
        job->setKey(name);
        break;
    case Ldap:
        job->setName(u"ldapclient"_s);
        job->setPassword(password);
        job->setKey(name);
    }
    job->start();
}
