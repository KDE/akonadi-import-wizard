/*
   SPDX-FileCopyrightText: 2012-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "evolutionsettings.h"
#include "evolutionutil.h"
#include "importwizardutil.h"
#include <MailCommon/MailUtil>

#include <KIdentityManagementCore/Identity>

#include <MailTransport/TransportManager>

#include "evolutionv3plugin_debug.h"

#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>

EvolutionSettings::EvolutionSettings() = default;

EvolutionSettings::~EvolutionSettings() = default;

void EvolutionSettings::loadAccount(const QString &filename)
{
    // Read gconf file
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qCDebug(EVOLUTIONPLUGIN_LOG) << " We can't open file" << filename;
        return;
    }
    QDomDocument doc;
    if (!EvolutionUtil::loadInDomDocument(&file, doc)) {
        return;
    }
    QDomElement config = doc.documentElement();

    if (config.isNull()) {
        qCDebug(EVOLUTIONPLUGIN_LOG) << "No config found in filename " << filename;
        return;
    }
    for (QDomElement e = config.firstChildElement(); !e.isNull(); e = e.nextSiblingElement()) {
        const QString tag = e.tagName();
        if (tag == QLatin1StringView("entry")) {
            if (e.hasAttribute(QStringLiteral("name"))) {
                const QString attr = e.attribute(QStringLiteral("name"));
                if (attr == QLatin1StringView("accounts")) {
                    readAccount(e);
                } else if (attr == QLatin1StringView("signatures")) {
                    readSignatures(e);
                } else if (attr == QLatin1StringView("send_recv_all_on_start")) {
                    // TODO: implement it.
                } else if (attr == QLatin1StringView("send_recv_on_start")) {
                    // TODO: implement it.
                } else {
                    qCDebug(EVOLUTIONPLUGIN_LOG) << " attr unknown " << attr;
                }
            }
        }
    }
}

void EvolutionSettings::loadLdap(const QString &filename)
{
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)) {
        qCDebug(EVOLUTIONPLUGIN_LOG) << " We can't open file" << filename;
        return;
    }
    QDomDocument doc;
    if (!EvolutionUtil::loadInDomDocument(&file, doc)) {
        return;
    }
    QDomElement ldapConfig = doc.documentElement();
    for (QDomElement e = ldapConfig.firstChildElement(); !e.isNull(); e = e.nextSiblingElement()) {
        const QString tag = e.tagName();
        if (tag == QLatin1StringView("entry")) {
            for (QDomElement serverConfig = e.firstChildElement(); !serverConfig.isNull(); serverConfig = serverConfig.nextSiblingElement()) {
                if (serverConfig.tagName() == QLatin1StringView("li")) {
                    QDomElement ldapValue = serverConfig.firstChildElement();
                    readLdap(ldapValue.text());
                }
            }
        }
    }
}

void EvolutionSettings::readLdap(const QString &ldapStr)
{
    qCDebug(EVOLUTIONPLUGIN_LOG) << " ldap " << ldapStr;
    QDomDocument ldap;
    if (!EvolutionUtil::loadInDomDocument(ldapStr, ldap)) {
        return;
    }

    QDomElement domElement = ldap.documentElement();

    if (domElement.isNull()) {
        qCDebug(EVOLUTIONPLUGIN_LOG) << "ldap not found";
        return;
    }
    // Ldap server
    if (domElement.attribute(QStringLiteral("base_uri")) == QLatin1StringView("ldap://")) {
        for (QDomElement e = domElement.firstChildElement(); !e.isNull(); e = e.nextSiblingElement()) {
            // const QString name = e.attribute( QLatin1StringView( "name" ) ); We don't use it in kmail

            ldapStruct ldap;
            const QString relative_uri = e.attribute(QStringLiteral("relative_uri"));
            const QString uri = e.attribute(QStringLiteral("uri"));
            QUrl url(uri);
            ldap.port = url.port();
            ldap.ldapUrl = url;
            qCDebug(EVOLUTIONPLUGIN_LOG) << " relative_uri" << relative_uri;

            QDomElement propertiesElement = e.firstChildElement();
            if (!propertiesElement.isNull()) {
                for (QDomElement property = propertiesElement.firstChildElement(); !property.isNull(); property = property.nextSiblingElement()) {
                    const QString propertyTag = property.tagName();
                    if (propertyTag == QLatin1StringView("property")) {
                        if (property.hasAttribute(QStringLiteral("name"))) {
                            const QString propertyName = property.attribute(QStringLiteral("name"));
                            if (propertyName == QLatin1StringView("timeout")) {
                                ldap.timeout = property.attribute(QStringLiteral("value")).toInt();
                            } else if (propertyName == QLatin1StringView("ssl")) {
                                const QString value = property.attribute(QStringLiteral("value"));
                                if (value == QLatin1StringView("always")) {
                                    ldap.useSSL = true;
                                } else if (value == QLatin1StringView("whenever_possible")) {
                                    ldap.useTLS = true;
                                } else {
                                    qCDebug(EVOLUTIONPLUGIN_LOG) << " ssl attribute unknown" << value;
                                }
                            } else if (propertyName == QLatin1StringView("limit")) {
                                ldap.limit = property.attribute(QStringLiteral("value")).toInt();
                            } else if (propertyName == QLatin1StringView("binddn")) {
                                ldap.dn = property.attribute(QStringLiteral("value"));
                            } else if (propertyName == QLatin1StringView("auth")) {
                                const QString value = property.attribute(QStringLiteral("value"));
                                if (value == QLatin1StringView("ldap/simple-email")) {
                                    // TODO:
                                } else if (value == QLatin1StringView("none")) {
                                    // TODO:
                                } else if (value == QLatin1StringView("ldap/simple-binddn")) {
                                    // TODO:
                                } else {
                                    qCDebug(EVOLUTIONPLUGIN_LOG) << " Unknown auth value " << value;
                                }
                                qCDebug(EVOLUTIONPLUGIN_LOG) << " auth" << value;
                            } else {
                                qCDebug(EVOLUTIONPLUGIN_LOG) << " property unknown :" << propertyName;
                            }
                        }
                    } else {
                        qCDebug(EVOLUTIONPLUGIN_LOG) << " tag unknown :" << propertyTag;
                    }
                }
                ImportWizardUtil::mergeLdap(ldap);
            }
        }
    }
}

void EvolutionSettings::readSignatures(const QDomElement &account)
{
    for (QDomElement signatureConfig = account.firstChildElement(); !signatureConfig.isNull(); signatureConfig = signatureConfig.nextSiblingElement()) {
        if (signatureConfig.tagName() == QLatin1StringView("li")) {
            QDomElement stringValue = signatureConfig.firstChildElement();
            extractSignatureInfo(stringValue.text());
        }
    }
}

void EvolutionSettings::extractSignatureInfo(const QString &info)
{
    // qCDebug(EVOLUTIONPLUGIN_LOG)<<" signature info "<<info;
    QDomDocument signature;
    if (!EvolutionUtil::loadInDomDocument(info, signature)) {
        return;
    }

    QDomElement domElement = signature.documentElement();

    if (domElement.isNull()) {
        qCDebug(EVOLUTIONPLUGIN_LOG) << "Signature not found";
        return;
    }
    for (QDomElement e = domElement.firstChildElement(); !e.isNull(); e = e.nextSiblingElement()) {
        KIdentityManagementCore::Signature signature;

        const QString tag = e.tagName();
        const QString uid = e.attribute(QStringLiteral("uid"));
        const QString signatureName = e.attribute(QStringLiteral("name")); // Use it ?
        const QString format = e.attribute(QStringLiteral("text"));
        const bool automatic = (e.attribute(QStringLiteral("auto")) == QLatin1StringView("true"));
        if (automatic) {
            // TODO:
        } else {
            if (format == QLatin1StringView("text/html")) {
                signature.setInlinedHtml(true);
            } else if (format == QLatin1StringView("text/plain")) {
                signature.setInlinedHtml(false);
            }

            if (tag == QLatin1StringView("filename")) {
                if (e.hasAttribute(QStringLiteral("script")) && e.attribute(QStringLiteral("script")) == QLatin1StringView("true")) {
                    signature.setPath(e.text(), true);
                    signature.setType(KIdentityManagementCore::Signature::FromCommand);
                } else {
                    signature.setPath(QDir::homePath() + QLatin1StringView("/.local/share/evolution/signatures/") + e.text(), false);
                    signature.setType(KIdentityManagementCore::Signature::FromFile);
                }
            }
        }
        mMapSignature.insert(uid, signature);

        qCDebug(EVOLUTIONPLUGIN_LOG) << " signature tag :" << tag;
    }
}

void EvolutionSettings::readAccount(const QDomElement &account)
{
    for (QDomElement accountConfig = account.firstChildElement(); !accountConfig.isNull(); accountConfig = accountConfig.nextSiblingElement()) {
        if (accountConfig.tagName() == QLatin1StringView("li")) {
            QDomElement stringValue = accountConfig.firstChildElement();
            extractAccountInfo(stringValue.text());
        }
    }
}

void EvolutionSettings::extractAccountInfo(const QString &info)
{
    qCDebug(EVOLUTIONPLUGIN_LOG) << " info " << info;
    // Read QDomElement
    QDomDocument account;
    if (!EvolutionUtil::loadInDomDocument(info, account)) {
        return;
    }

    QDomElement domElement = account.documentElement();

    if (domElement.isNull()) {
        qCDebug(EVOLUTIONPLUGIN_LOG) << "Account not found";
        return;
    }

    QString name;
    if (domElement.hasAttribute(QStringLiteral("name"))) {
        name = domElement.attribute(QStringLiteral("name"));
    }

    KIdentityManagementCore::Identity *newIdentity = createIdentity(name);

    const bool enableManualCheck = (domElement.attribute(QStringLiteral("enabled")) == QLatin1StringView("true"));

    for (QDomElement e = domElement.firstChildElement(); !e.isNull(); e = e.nextSiblingElement()) {
        const QString tag = e.tagName();
        if (tag == QLatin1StringView("identity")) {
            for (QDomElement identity = e.firstChildElement(); !identity.isNull(); identity = identity.nextSiblingElement()) {
                const QString identityTag = identity.tagName();
                if (identityTag == QLatin1StringView("name")) {
                    const QString fullName(identity.text());
                    newIdentity->setIdentityName(fullName);
                    newIdentity->setFullName(fullName);
                } else if (identityTag == QLatin1StringView("addr-spec")) {
                    newIdentity->setPrimaryEmailAddress(identity.text());
                } else if (identityTag == QLatin1StringView("organization")) {
                    newIdentity->setOrganization(identity.text());
                } else if (identityTag == QLatin1StringView("signature")) {
                    if (identity.hasAttribute(QStringLiteral("uid"))) {
                        newIdentity->setSignature(mMapSignature.value(identity.attribute(QStringLiteral("uid"))));
                    }
                } else if (identityTag == QLatin1StringView("reply-to")) {
                    newIdentity->setReplyToAddr(identity.text());
                } else {
                    qCDebug(EVOLUTIONPLUGIN_LOG) << " tag identity not found :" << identityTag;
                }
            }
        } else if (tag == QLatin1StringView("source")) {
            if (e.hasAttribute(QStringLiteral("save-passwd")) && e.attribute(QStringLiteral("save-passwd")) == QLatin1StringView("true")) {
                // TODO
            }
            int interval = -1;
            bool intervalCheck = false;
            if (e.hasAttribute(QStringLiteral("auto-check"))) {
                intervalCheck = (e.attribute(QStringLiteral("auto-check")) == QLatin1StringView("true"));
            }
            if (e.hasAttribute(QStringLiteral("auto-check-timeout"))) {
                interval = e.attribute(QStringLiteral("auto-check-timeout")).toInt();
            }
            for (QDomElement server = e.firstChildElement(); !server.isNull(); server = server.nextSiblingElement()) {
                const QString serverTag = server.tagName();
                if (serverTag == QLatin1StringView("url")) {
                    qCDebug(EVOLUTIONPLUGIN_LOG) << " server.text() :" << server.text();
                    QUrl serverUrl(server.text());
                    const QString scheme = serverUrl.scheme();
                    QMap<QString, QVariant> settings;
                    const int port = serverUrl.port();

                    const QString path = serverUrl.path();
                    qCDebug(EVOLUTIONPLUGIN_LOG) << " path !" << path;
                    const QString userName = serverUrl.userInfo();

                    const QStringList listArgument = path.split(QLatin1Char(';'));

                    // imapx://name@pop3.xx.org:993/;security-method=ssl-on-alternate-port;namespace;shell-command=ssh%20-C%20-l%20%25u%20%25h%20exec%20/usr/sbin/imapd%20;use-shell-command=true
                    if (scheme == QLatin1StringView("imap") || scheme == QLatin1StringView("imapx")) {
                        if (port > 0) {
                            settings.insert(QStringLiteral("ImapPort"), port);
                        }
                        // Perhaps imapx is specific don't know
                        if (intervalCheck) {
                            settings.insert(QStringLiteral("IntervalCheckEnabled"), true);
                        }
                        if (interval > -1) {
                            settings.insert(QStringLiteral("IntervalCheckTime"), interval);
                        }

                        bool found = false;
                        const QString securityMethod = getSecurityMethod(listArgument, found);
                        if (found) {
                            if (securityMethod == QLatin1StringView("none")) {
                                settings.insert(QStringLiteral("Safety"), QStringLiteral("None"));
                                // Nothing
                            } else if (securityMethod == QLatin1StringView("ssl-on-alternate-port")) {
                                settings.insert(QStringLiteral("Safety"), QStringLiteral("SSL"));
                            } else {
                                qCDebug(EVOLUTIONPLUGIN_LOG) << " security method unknown : " << path;
                            }
                        } else {
                            settings.insert(QStringLiteral("Safety"), QStringLiteral("STARTTLS"));
                        }

                        addAuth(settings, QStringLiteral("Authentication"), userName);
                        const QString agentIdentifyName =
                            LibImportWizard::AbstractBase::createResource(QStringLiteral("akonadi_imap_resource"), name, settings);
                        // By default
                        addCheckMailOnStartup(agentIdentifyName, enableManualCheck);
                        addToManualCheck(agentIdentifyName, enableManualCheck);
                    } else if (scheme == QLatin1StringView("pop")) {
                        if (port > 0) {
                            settings.insert(QStringLiteral("Port"), port);
                        }
                        bool found = false;
                        const QString securityMethod = getSecurityMethod(listArgument, found);
                        if (found) {
                            if (securityMethod == QLatin1StringView("none")) {
                                // Nothing
                            } else if (securityMethod == QLatin1StringView("ssl-on-alternate-port")) {
                                settings.insert(QStringLiteral("UseSSL"), true);
                            } else {
                                qCDebug(EVOLUTIONPLUGIN_LOG) << " security method unknown : " << path;
                            }
                        } else {
                            settings.insert(QStringLiteral("UseTLS"), true);
                        }

                        if (intervalCheck) {
                            settings.insert(QStringLiteral("IntervalCheckEnabled"), true);
                        }
                        if (interval > -1) {
                            settings.insert(QStringLiteral("IntervalCheckInterval"), interval);
                        }
                        if (e.hasAttribute(QStringLiteral("keep-on-server")) && e.attribute(QStringLiteral("keep-on-server")) == QLatin1StringView("true")) {
                            settings.insert(QStringLiteral("LeaveOnServer"), true);
                        }
                        addAuth(settings, QStringLiteral("AuthenticationMethod"), userName);
                        const QString agentIdentifyName =
                            LibImportWizard::AbstractBase::createResource(QStringLiteral("akonadi_pop3_resource"), name, settings);
                        // By default
                        addCheckMailOnStartup(agentIdentifyName, enableManualCheck);
                        addToManualCheck(agentIdentifyName, enableManualCheck);
                    } else if (scheme == QLatin1StringView("spool") || scheme == QLatin1StringView("mbox")) {
                        // mbox file
                        settings.insert(QStringLiteral("Path"), path);
                        settings.insert(QStringLiteral("DisplayName"), name);
                        LibImportWizard::AbstractBase::createResource(QStringLiteral("akonadi_mbox_resource"), name, settings);
                    } else if (scheme == QLatin1StringView("maildir") || scheme == QLatin1StringView("spooldir")) {
                        settings.insert(QStringLiteral("Path"), path);
                        LibImportWizard::AbstractBase::createResource(QStringLiteral("akonadi_maildir_resource"), name, settings);
                    } else if (scheme == QLatin1StringView("nntp")) {
                        // FIXME in the future
                        qCDebug(EVOLUTIONPLUGIN_LOG) << " For the moment we can't import nntp resource";
                    } else {
                        qCDebug(EVOLUTIONPLUGIN_LOG) << " unknown scheme " << scheme;
                    }
                } else {
                    qCDebug(EVOLUTIONPLUGIN_LOG) << " server tag unknown :" << serverTag;
                }
            }
        } else if (tag == QLatin1StringView("transport")) {
            if (e.hasAttribute(QStringLiteral("save-passwd")) && e.attribute(QStringLiteral("save-passwd")) == QLatin1StringView("true")) {
                // TODO save to kwallet ?
            }

            MailTransport::Transport *transport = createTransport();
            transport->setIdentifier(QStringLiteral("SMTP"));
            for (QDomElement smtp = e.firstChildElement(); !smtp.isNull(); smtp = smtp.nextSiblingElement()) {
                const QString smtpTag = smtp.tagName();
                if (smtpTag == QLatin1StringView("url")) {
                    qCDebug(EVOLUTIONPLUGIN_LOG) << " smtp.text() :" << smtp.text();
                    QUrl smtpUrl(smtp.text());
                    const QString scheme = smtpUrl.scheme();
                    if (scheme != QLatin1StringView("sendmail")) {
                        transport->setHost(smtpUrl.host());
                        transport->setName(smtpUrl.host());
                        // TODO setUserName :
                        // transport->setRequiresAuthentication(true);
                        // transport->setUserName(....);
                        const int port = smtpUrl.port();
                        if (port > 0) {
                            transport->setPort(port);
                        }

                        const QString userName = smtpUrl.userInfo();
                        bool found = false;
                        const QString authMethod = getAuthMethod(userName, found);
                        if (found) {
                            if (authMethod == QLatin1StringView("PLAIN")) {
                                transport->setAuthenticationType(MailTransport::Transport::EnumAuthenticationType::PLAIN);
                            } else if (authMethod == QLatin1StringView("NTLM")) {
                                transport->setAuthenticationType(MailTransport::Transport::EnumAuthenticationType::NTLM);
                            } else if (authMethod == QLatin1StringView("DIGEST-MD5")) {
                                transport->setAuthenticationType(MailTransport::Transport::EnumAuthenticationType::DIGEST_MD5);
                            } else if (authMethod == QLatin1StringView("CRAM-MD5")) {
                                transport->setAuthenticationType(MailTransport::Transport::EnumAuthenticationType::CRAM_MD5);
                            } else if (authMethod == QLatin1StringView("LOGIN")) {
                                transport->setAuthenticationType(MailTransport::Transport::EnumAuthenticationType::LOGIN);
                            } else if (authMethod == QLatin1StringView("GSSAPI")) {
                                transport->setAuthenticationType(MailTransport::Transport::EnumAuthenticationType::GSSAPI);
                            } else if (authMethod == QLatin1StringView("POPB4SMTP")) {
                                transport->setAuthenticationType(MailTransport::Transport::EnumAuthenticationType::APOP); //????
                            } else {
                                qCDebug(EVOLUTIONPLUGIN_LOG) << " smtp auth method unknown " << authMethod;
                            }
                        }

                        const QString path = smtpUrl.path();
                        found = false;
                        const QStringList listArgument = path.split(QLatin1Char(';'));
                        const QString securityMethod = getSecurityMethod(listArgument, found);
                        if (found) {
                            if (securityMethod == QLatin1StringView("none")) {
                                transport->setEncryption(MailTransport::Transport::EnumEncryption::None);
                            } else if (securityMethod == QLatin1StringView("ssl-on-alternate-port")) {
                                transport->setEncryption(MailTransport::Transport::EnumEncryption::SSL);
                            } else {
                                qCDebug(EVOLUTIONPLUGIN_LOG) << " security method unknown : " << path;
                            }
                        } else {
                            transport->setEncryption(MailTransport::Transport::EnumEncryption::TLS);
                        }
                    }
                } else {
                    qCDebug(EVOLUTIONPLUGIN_LOG) << " smtp tag unknown :" << smtpTag;
                }
            }
            storeTransport(transport, true);
        } else if (tag == QLatin1StringView("drafts-folder")) {
            const QString selectedFolder = MailCommon::Util::convertFolderPathToCollectionStr(e.text().remove(QStringLiteral("folder://")));
            newIdentity->setDrafts(selectedFolder);
        } else if (tag == QLatin1StringView("sent-folder")) {
            const QString selectedFolder = MailCommon::Util::convertFolderPathToCollectionStr(e.text().remove(QStringLiteral("folder://")));
            newIdentity->setFcc(selectedFolder);
        } else if (tag == QLatin1StringView("auto-cc")) {
            if (e.hasAttribute(QStringLiteral("always")) && (e.attribute(QStringLiteral("always")) == QLatin1StringView("true"))) {
                QDomElement recipient = e.firstChildElement();
                const QString text = recipient.text();
                newIdentity->setCc(text);
            }
        } else if (tag == QLatin1StringView("reply-to")) {
            newIdentity->setReplyToAddr(e.text());
        } else if (tag == QLatin1StringView("auto-bcc")) {
            if (e.hasAttribute(QStringLiteral("always")) && (e.attribute(QStringLiteral("always")) == QLatin1StringView("true"))) {
                QDomElement recipient = e.firstChildElement();
                const QString text = recipient.text();
                newIdentity->setBcc(text);
            }
        } else if (tag == QLatin1StringView("receipt-policy")) {
            if (e.hasAttribute(QStringLiteral("policy"))) {
                const QString policy = e.attribute(QStringLiteral("policy"));
                // TODO
            }
        } else if (tag == QLatin1StringView("pgp")) {
            if (e.hasAttribute(QStringLiteral("encrypt-to-self")) && (e.attribute(QStringLiteral("encrypt-to-self")) == QLatin1StringView("true"))) {
                // TODO
            }
            if (e.hasAttribute(QStringLiteral("always-trust")) && (e.attribute(QStringLiteral("always-trust")) == QLatin1StringView("true"))) {
                // TODO
            }
            if (e.hasAttribute(QStringLiteral("always-sign")) && (e.attribute(QStringLiteral("always-sign")) == QLatin1StringView("true"))) {
                // TODO
            }
            if (e.hasAttribute(QStringLiteral("no-imip-sign")) && (e.attribute(QStringLiteral("no-imip-sign")) == QLatin1StringView("true"))) {
                // TODO
            }
        } else if (tag == QLatin1StringView("smime")) {
            if (e.hasAttribute(QStringLiteral("sign-default")) && (e.attribute(QStringLiteral("sign-default")) == QLatin1StringView("true"))) {
                // TODO
            }
            if (e.hasAttribute(QStringLiteral("encrypt-default")) && (e.attribute(QStringLiteral("encrypt-default")) == QLatin1StringView("true"))) {
                // TODO
            }
            if (e.hasAttribute(QStringLiteral("encrypt-to-self")) && (e.attribute(QStringLiteral("encrypt-to-self")) == QLatin1StringView("true"))) {
                // TODO
            }
            // TODO
        } else {
            qCDebug(EVOLUTIONPLUGIN_LOG) << " tag not know :" << tag;
        }
    }
    storeIdentity(newIdentity);
}

QString EvolutionSettings::getSecurityMethod(const QStringList &listArgument, bool &found)
{
    found = false;
    if (listArgument.isEmpty()) {
        return {};
    }
    for (const QString &str : listArgument) {
        if (str.contains(QLatin1StringView("security-method="))) {
            const int index = str.indexOf(QLatin1StringView("security-method="));
            if (index != -1) {
                const QString securityMethod = str.right(str.length() - index - 16 /*security-method=*/);
                found = true;
                return securityMethod;
            }
        }
    }
    return {};
}

QString EvolutionSettings::getAuthMethod(const QString &path, bool &found)
{
    const int index = path.indexOf(QLatin1StringView("auth="));
    if (index != -1) {
        const QString securityMethod = path.right(path.length() - index - 5 /*auth=*/);
        found = true;
        return securityMethod;
    }
    found = false;
    return {};
}

void EvolutionSettings::addAuth(QMap<QString, QVariant> &settings, const QString &argument, const QString &userName)
{
    bool found = false;
    const QString authMethod = getAuthMethod(userName, found);
    if (found) {
        if (authMethod == QLatin1StringView("PLAIN")) {
            settings.insert(argument, MailTransport::Transport::EnumAuthenticationType::PLAIN);
        } else if (authMethod == QLatin1StringView("NTLM")) {
            settings.insert(argument, MailTransport::Transport::EnumAuthenticationType::NTLM);
        } else if (authMethod == QLatin1StringView("DIGEST-MD5")) {
            settings.insert(argument, MailTransport::Transport::EnumAuthenticationType::DIGEST_MD5);
        } else if (authMethod == QLatin1StringView("CRAM-MD5")) {
            settings.insert(argument, MailTransport::Transport::EnumAuthenticationType::CRAM_MD5);
        } else if (authMethod == QLatin1StringView("LOGIN")) {
            settings.insert(argument, MailTransport::Transport::EnumAuthenticationType::LOGIN);
        } else if (authMethod == QLatin1StringView("POPB4SMTP")) {
            settings.insert(argument, MailTransport::Transport::EnumAuthenticationType::APOP); //????
        } else {
            qCDebug(EVOLUTIONPLUGIN_LOG) << " smtp auth method unknown " << authMethod;
        }
    }
}
