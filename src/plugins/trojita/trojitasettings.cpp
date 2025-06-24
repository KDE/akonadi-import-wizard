/*
   SPDX-FileCopyrightText: 2012-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "trojitasettings.h"
using namespace Qt::Literals::StringLiterals;

#include <ImportWizard/ImportWizardUtil>

#include <MailTransport/TransportManager>

#include <KIdentityManagementCore/Identity>
#include <KIdentityManagementCore/Signature>

#include "trojitaplugin_debug.h"

#include <QSettings>

TrojitaSettings::TrojitaSettings(const QString &filename)
{
    settings = new QSettings(filename, QSettings::IniFormat, this);
}

TrojitaSettings::~TrojitaSettings()
{
    delete settings;
}

void TrojitaSettings::importSettings()
{
    readImapAccount();
    readTransport();
    readIdentity();
    readGlobalSettings();
}

void TrojitaSettings::readImapAccount()
{
    QMap<QString, QVariant> newSettings;
    QString name;

    name = settings->value(u"imap.host"_s).toString();
    if (!name.isEmpty()) {
        newSettings.insert(u"ImapServer"_s, name);
    }

    const int port = settings->value(u"imap.port"_s, -1).toInt();
    if (port > -1) {
        newSettings.insert(u"ImapPort"_s, port);
    }

    if (settings->contains(u"imap.starttls"_s)) {
        const bool useTLS = settings->value(u"imap.starttls"_s).toBool();
        if (useTLS) {
            newSettings.insert(u"Safety"_s, u"STARTTLS"_s);
        }
    }

    const QString userName = settings->value(u"imap.auth.user"_s).toString();
    if (!userName.isEmpty()) {
        newSettings.insert(u"Username"_s, userName);
    }
    const QString password = settings->value(u"imap.auth.pass"_s).toString();
    if (!password.isEmpty()) {
        newSettings.insert(u"Password"_s, password);
    }

#if 0
    if (settings->contains(u"imap.process"_s)) {
        //What's this ?
    }

    if (settings->contains(u"imap.offline"_s)) {
        //const bool offlineStatus = settings->value(u"imap.offline"_s).toBool();
        //It's not a deconnected mode as imap disconnected #317023
        //Will implement soon.
        //TODO use akonadi cache.
    }
    if (settings->contains(u"imap.enableId"_s)) {
        //Not supported by Akonadi.
    }

    if (settings->contains(u"imap.ssl.pemCertificate"_s)) {
        //Not supported by akonadi.
    }

    if (settings->contains(u"imap.capabilities.blacklist"_s)) {
        //Not supported by akonadi-imap-resource.
    }
#endif
    if (!name.isEmpty()) {
        const QString agentIdentifyName = LibImportWizard::AbstractBase::createResource(u"akonadi_imap_resource"_s, name, newSettings);
        // Check by default
        addCheckMailOnStartup(agentIdentifyName, true);
    }
}

void TrojitaSettings::readTransport()
{
    settings->beginGroup(u"General"_s);
    const QString smtpMethod = settings->value(u"msa.method"_s).toString();
    if (!smtpMethod.isEmpty()) {
        MailTransport::Transport *mt = createTransport();
        if (smtpMethod == QLatin1StringView("IMAP-SENDMAIL")) {
            // see http://tools.ietf.org/html/draft-kundrat-imap-submit-01
        } else if (smtpMethod == QLatin1StringView("SMTP") || smtpMethod == QLatin1StringView("SSMTP")) {
            if (settings->contains(u"msa.smtp.host"_s)) {
                mt->setHost(settings->value(u"msa.smtp.host"_s).toString());
            }
            if (settings->contains(u"msa.smtp.port"_s)) {
                mt->setPort(settings->value(u"msa.smtp.port"_s).toInt());
            }
            if (settings->contains(u"msa.smtp.auth"_s)) {
                if (settings->value(u"msa.smtp.auth"_s).toBool()) {
                    if (settings->contains(u"msa.smtp.auth.user"_s)) {
                        mt->setUserName(settings->value(u"msa.smtp.auth.user"_s).toString());
                    }
                    if (settings->contains(u"msa.smtp.auth.pass"_s)) {
                        mt->setPassword(settings->value(u"msa.smtp.auth.pass"_s).toString());
                    }
                }
            }

            if (settings->contains(u"msa.smtp.starttls"_s)) {
                if (settings->value(u"msa.smtp.starttls"_s).toBool()) {
                    mt->setEncryption(MailTransport::Transport::EnumEncryption::TLS);
                }
            }
            mt->setIdentifier(u"SMTP"_s);
        } else {
            qCWarning(TROJITAPLUGIN_LOG) << " smtpMethod unknown " << smtpMethod;
        }
        storeTransport(mt, true); // only one smtp for the moment
    }
    settings->endGroup();
}

void TrojitaSettings::readIdentity()
{
    const int size = settings->beginReadArray(u"identities"_s);
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        QString realName = settings->value(u"realName"_s).toString();
        KIdentityManagementCore::Identity *identity = createIdentity(realName);
        identity->setFullName(realName);
        identity->setIdentityName(realName);
        const QString address = settings->value(u"address"_s).toString();
        identity->setPrimaryEmailAddress(address);
        const QString organisation = settings->value(u"organisation"_s).toString();
        identity->setOrganization(organisation);
        QString signatureStr = settings->value(u"signature"_s).toString();
        if (!signatureStr.isEmpty()) {
            KIdentityManagementCore::Signature signature;
            signature.setType(KIdentityManagementCore::Signature::Inlined);
            signature.setText(signatureStr);
            identity->setSignature(signature);
        }
        qCDebug(TROJITAPLUGIN_LOG) << " realName :" << realName << " address : " << address << " organisation : " << organisation
                                   << " signature: " << signatureStr;
        storeIdentity(identity);
    }
    settings->endArray();
}

void TrojitaSettings::readGlobalSettings()
{
    // TODO
}
