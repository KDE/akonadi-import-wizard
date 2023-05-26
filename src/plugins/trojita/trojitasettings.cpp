/*
   SPDX-FileCopyrightText: 2012-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "trojitasettings.h"
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

    name = settings->value(QStringLiteral("imap.host")).toString();
    if (!name.isEmpty()) {
        newSettings.insert(QStringLiteral("ImapServer"), name);
    }

    const int port = settings->value(QStringLiteral("imap.port"), -1).toInt();
    if (port > -1) {
        newSettings.insert(QStringLiteral("ImapPort"), port);
    }

    if (settings->contains(QStringLiteral("imap.starttls"))) {
        const bool useTLS = settings->value(QStringLiteral("imap.starttls")).toBool();
        if (useTLS) {
            newSettings.insert(QStringLiteral("Safety"), QStringLiteral("STARTTLS"));
        }
    }

    const QString userName = settings->value(QStringLiteral("imap.auth.user")).toString();
    if (!userName.isEmpty()) {
        newSettings.insert(QStringLiteral("Username"), userName);
    }
    const QString password = settings->value(QStringLiteral("imap.auth.pass")).toString();
    if (!password.isEmpty()) {
        newSettings.insert(QStringLiteral("Password"), password);
    }

#if 0
    if (settings->contains(QStringLiteral("imap.process"))) {
        //What's this ?
    }

    if (settings->contains(QStringLiteral("imap.offline"))) {
        //const bool offlineStatus = settings->value(QStringLiteral("imap.offline")).toBool();
        //It's not a deconnected mode as imap disconnected #317023
        //Will implement soon.
        //TODO use akonadi cache.
    }
    if (settings->contains(QStringLiteral("imap.enableId"))) {
        //Not supported by Akonadi.
    }

    if (settings->contains(QStringLiteral("imap.ssl.pemCertificate"))) {
        //Not supported by akonadi.
    }

    if (settings->contains(QStringLiteral("imap.capabilities.blacklist"))) {
        //Not supported by akonadi-imap-resource.
    }
#endif
    if (!name.isEmpty()) {
        const QString agentIdentifyName = LibImportWizard::AbstractBase::createResource(QStringLiteral("akonadi_imap_resource"), name, newSettings);
        // Check by default
        addCheckMailOnStartup(agentIdentifyName, true);
    }
}

void TrojitaSettings::readTransport()
{
    settings->beginGroup(QStringLiteral("General"));
    const QString smtpMethod = settings->value(QStringLiteral("msa.method")).toString();
    if (!smtpMethod.isEmpty()) {
        MailTransport::Transport *mt = createTransport();
        if (smtpMethod == QLatin1String("IMAP-SENDMAIL")) {
            // see http://tools.ietf.org/html/draft-kundrat-imap-submit-01
        } else if (smtpMethod == QLatin1String("SMTP") || smtpMethod == QLatin1String("SSMTP")) {
            if (settings->contains(QStringLiteral("msa.smtp.host"))) {
                mt->setHost(settings->value(QStringLiteral("msa.smtp.host")).toString());
            }
            if (settings->contains(QStringLiteral("msa.smtp.port"))) {
                mt->setPort(settings->value(QStringLiteral("msa.smtp.port")).toInt());
            }
            if (settings->contains(QStringLiteral("msa.smtp.auth"))) {
                if (settings->value(QStringLiteral("msa.smtp.auth")).toBool()) {
                    if (settings->contains(QStringLiteral("msa.smtp.auth.user"))) {
                        mt->setUserName(settings->value(QStringLiteral("msa.smtp.auth.user")).toString());
                    }
                    if (settings->contains(QStringLiteral("msa.smtp.auth.pass"))) {
                        mt->setPassword(settings->value(QStringLiteral("msa.smtp.auth.pass")).toString());
                    }
                }
            }

            if (settings->contains(QStringLiteral("msa.smtp.starttls"))) {
                if (settings->value(QStringLiteral("msa.smtp.starttls")).toBool()) {
                    mt->setEncryption(MailTransport::Transport::EnumEncryption::TLS);
                }
            }
            mt->setIdentifier(QStringLiteral("SMTP"));
        } else {
            qCWarning(TROJITAPLUGIN_LOG) << " smtpMethod unknown " << smtpMethod;
        }
        storeTransport(mt, true); // only one smtp for the moment
    }
    settings->endGroup();
}

void TrojitaSettings::readIdentity()
{
    const int size = settings->beginReadArray(QStringLiteral("identities"));
    for (int i = 0; i < size; ++i) {
        settings->setArrayIndex(i);
        QString realName = settings->value(QStringLiteral("realName")).toString();
        KIdentityManagementCore::Identity *identity = createIdentity(realName);
        identity->setFullName(realName);
        identity->setIdentityName(realName);
        const QString address = settings->value(QStringLiteral("address")).toString();
        identity->setPrimaryEmailAddress(address);
        const QString organisation = settings->value(QStringLiteral("organisation")).toString();
        identity->setOrganization(organisation);
        QString signatureStr = settings->value(QStringLiteral("signature")).toString();
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
