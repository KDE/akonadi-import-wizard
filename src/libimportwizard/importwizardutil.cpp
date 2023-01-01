/*
   SPDX-FileCopyrightText: 2012-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "importwizardutil.h"
#include "importwizardsavepasswordjob.h"
#include "libimportwizard_debug.h"
#include <Akonadi/Tag>
#include <Akonadi/TagAttribute>
#include <Akonadi/TagCreateJob>
#include <KConfigGroup>
#include <KSharedConfig>

void ImportWizardUtil::mergeLdap(const ldapStruct &ldap)
{
    KSharedConfigPtr ldapConfig = KSharedConfig::openConfig(QStringLiteral("kabldaprc"));
    int numberOfLdapSelected = 0;
    KConfigGroup grp;
    if (ldapConfig->hasGroup(QStringLiteral("LDAP"))) {
        grp = ldapConfig->group(QStringLiteral("LDAP"));
        numberOfLdapSelected = grp.readEntry(QStringLiteral("NumSelectedHosts"), 0);
        grp.writeEntry(QStringLiteral("NumSelectedHosts"), (numberOfLdapSelected + 1));
    } else {
        grp = ldapConfig->group(QStringLiteral("LDAP"));
        grp.writeEntry(QStringLiteral("NumSelectedHosts"), 1);

        KConfigGroup ldapSeach = ldapConfig->group(QStringLiteral("LDAPSearch"));
        ldapSeach.writeEntry(QStringLiteral("SearchType"), 0);
    }
    const int port = ldap.port;
    if (port != -1) {
        grp.writeEntry(QStringLiteral("SelectedPort%1").arg(numberOfLdapSelected), port);
    }
    grp.writeEntry(QStringLiteral("SelectedHost%1").arg(numberOfLdapSelected), ldap.ldapUrl.host());
    if (ldap.useSSL) {
        grp.writeEntry(QStringLiteral("SelectedSecurity%1").arg(numberOfLdapSelected), QStringLiteral("SSL"));
    } else if (ldap.useTLS) {
        grp.writeEntry(QStringLiteral("SelectedSecurity%1").arg(numberOfLdapSelected), QStringLiteral("TLS"));
    } else {
        grp.writeEntry(QStringLiteral("SelectedSecurity%1").arg(numberOfLdapSelected), QStringLiteral("None"));
    }

    if (ldap.saslMech == QLatin1String("GSSAPI")) {
        grp.writeEntry(QStringLiteral("SelectedMech%1").arg(numberOfLdapSelected), QStringLiteral("GSSAPI"));
        grp.writeEntry(QStringLiteral("SelectedAuth%1").arg(numberOfLdapSelected), QStringLiteral("SASL"));
    } else if (ldap.saslMech.isEmpty()) {
        grp.writeEntry(QStringLiteral("SelectedMech%1").arg(numberOfLdapSelected), QStringLiteral("PLAIN"));
        grp.writeEntry(QStringLiteral("SelectedAuth%1").arg(numberOfLdapSelected), QStringLiteral("Simple"));
    } else {
        qCDebug(LIBIMPORTWIZARD_LOG) << " Mech SASL undefined" << ldap.saslMech;
    }
    grp.writeEntry(QStringLiteral("SelectedVersion%1").arg(numberOfLdapSelected), QString::number(3));
    grp.writeEntry(QStringLiteral("SelectedBind%1").arg(numberOfLdapSelected), ldap.dn);
    // TODO: Verify selectedbase
    grp.writeEntry(QStringLiteral("SelectedBase%1").arg(numberOfLdapSelected), ldap.ldapUrl.path());
    if (ldap.timeout != -1) {
        grp.writeEntry(QStringLiteral("SelectedTimeLimit%1").arg(numberOfLdapSelected), ldap.timeout);
    }
    if (ldap.limit != -1) {
        grp.writeEntry(QStringLiteral("SelectedSizeLimit%1").arg(numberOfLdapSelected), ldap.limit);
    }
    if (!ldap.password.isEmpty()) {
        storePassword(QStringLiteral("SelectedPwdBind%1").arg(numberOfLdapSelected), ImportWizardUtil::Ldap, ldap.password);
    }
    grp.sync();
}

void ImportWizardUtil::addAkonadiTag(const QVector<tagStruct> &tagList)
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
        job->setName(QStringLiteral("imap"));
        job->setPassword(password);
        job->setKey(name + QLatin1String("rc"));
        break;
    case Pop3:
        job->setName(QStringLiteral("pop3"));
        job->setPassword(password);
        job->setKey(name);
        break;
    case Ldap:
        job->setName(QStringLiteral("ldapclient"));
        job->setPassword(password);
        job->setKey(name);
    }
    job->start();
}
