/*
   SPDX-FileCopyrightText: 2012-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "balsasettings.h"
#include "balsaplugin_debug.h"
#include <MailCommon/MailUtil>
#include <MailTransport/TransportManager>
#include <MessageViewer/KXFace>

#include <KIdentityManagementCore/Identity>
#include <KIdentityManagementCore/Signature>

#include <KConfig>
#include <KConfigGroup>
#include <QImage>
#include <QRegularExpression>

BalsaSettings::BalsaSettings(const QString &filename)
    : mFileName(filename)
{
}

BalsaSettings::~BalsaSettings() = default;

void BalsaSettings::importSettings()
{
    KConfig config(mFileName);

    bool autoCheck = false;
    int autoCheckDelay = -1;
    if (config.hasGroup(QStringLiteral("MailboxChecking"))) {
        KConfigGroup grp = config.group(QStringLiteral("MailboxChecking"));
        autoCheck = grp.readEntry(QStringLiteral("Auto"), false);
        autoCheckDelay = grp.readEntry(QStringLiteral("AutoDelay"), -1);
    }

    const QStringList mailBoxList = config.groupList().filter(QRegularExpression(QStringLiteral("mailbox-")));
    for (const QString &mailBox : mailBoxList) {
        KConfigGroup grp = config.group(mailBox);
        readAccount(grp, autoCheck, autoCheckDelay);
    }

    const QStringList smtpList = config.groupList().filter(QRegularExpression(QStringLiteral("smtp-server-")));
    for (const QString &smtp : smtpList) {
        KConfigGroup grp = config.group(smtp);
        readTransport(grp);
    }
    readGlobalSettings(config);
}

void BalsaSettings::readAccount(const KConfigGroup &grp, bool autoCheck, int autoDelay)
{
    Q_UNUSED(autoDelay)
    const QString type = grp.readEntry(QStringLiteral("Type"));
    bool check = grp.readEntry(QStringLiteral("Check"), false);
    if (type == QLatin1StringView("LibBalsaMailboxPOP3")) {
        QMap<QString, QVariant> settings;
        const QString server = grp.readEntry(QStringLiteral("Server"));
        settings.insert(QStringLiteral("Host"), server);
        const QString name = grp.readEntry(QStringLiteral("Name"));

        // const bool apop = grp.readEntry(QStringLiteral("DisableApop"), false);
        // Q_UNUSED(apop)
        const QString agentIdentifyName = LibImportWizard::AbstractBase::createResource(QStringLiteral("akonadi_pop3_resource"), name, settings);

        addCheckMailOnStartup(agentIdentifyName, autoCheck);
        addToManualCheck(agentIdentifyName, check);
    } else if (type == QLatin1StringView("LibBalsaMailboxImap")) {
        QMap<QString, QVariant> settings;
        const QString server = grp.readEntry(QStringLiteral("Server"));
        settings.insert(QStringLiteral("ImapServer"), server);
        const QString name = grp.readEntry(QStringLiteral("Name"));
        const QString agentIdentifyName = LibImportWizard::AbstractBase::createResource(QStringLiteral("akonadi_imap_resource"), name, settings);
        addCheckMailOnStartup(agentIdentifyName, autoCheck);
        addToManualCheck(agentIdentifyName, check);
    } else {
        qCDebug(BALSAPLUGIN_LOG) << " unknown account type :" << type;
    }
}

void BalsaSettings::readIdentity(const KConfigGroup &grp)
{
    QString name = grp.readEntry(QStringLiteral("FullName"));
    KIdentityManagementCore::Identity *newIdentity = createIdentity(name);
    newIdentity->setFullName(name);
    newIdentity->setPrimaryEmailAddress(grp.readEntry(QStringLiteral("Address")));
    newIdentity->setReplyToAddr(grp.readEntry(QStringLiteral("ReplyTo")));
    newIdentity->setBcc(grp.readEntry(QStringLiteral("Bcc")));
    const QString smtp = grp.readEntry(QStringLiteral("SmtpServer"));
    if (!smtp.isEmpty()) {
        const QString val = mHashSmtp.value(smtp);
        if (val.isEmpty()) {
            newIdentity->setTransport(val);
        }
    }

    const QString signaturePath = grp.readEntry(QStringLiteral("SignaturePath"));
    if (!signaturePath.isEmpty()) {
        KIdentityManagementCore::Signature signature;
        if (grp.readEntry(QStringLiteral("SigExecutable"), false)) {
            signature.setPath(signaturePath, true);
            signature.setType(KIdentityManagementCore::Signature::FromCommand);
        } else {
            signature.setType(KIdentityManagementCore::Signature::FromFile);
        }
        newIdentity->setSignature(signature);
    }

    const QString xfacePathStr = grp.readEntry(QStringLiteral("XFacePath"));
    if (!xfacePathStr.isEmpty()) {
        newIdentity->setXFaceEnabled(true);
        MessageViewer::KXFace xf;
        newIdentity->setXFace(xf.fromImage(QImage(xfacePathStr)));
    }
    storeIdentity(newIdentity);
}

void BalsaSettings::readTransport(const KConfigGroup &grp)
{
    MailTransport::Transport *mt = createTransport();
    const QString smtp = grp.name().remove(QStringLiteral("smtp-server-"));
    const QString server = grp.readEntry(QStringLiteral("Server"));
    mt->setHost(server);

    const int tlsMode = grp.readEntry(QStringLiteral("TLSMode"), -1);
    // TODO
    switch (tlsMode) {
    case 0:
        break;
    case 1:
        mt->setEncryption(MailTransport::Transport::EnumEncryption::TLS);
        break;
    case 2:
        break;
    default:
        break;
    }

    const QString ssl = grp.readEntry(QStringLiteral("SSL"));
    if (ssl == QLatin1StringView("true")) {
        mt->setEncryption(MailTransport::Transport::EnumEncryption::SSL);
    } else if (ssl == QLatin1StringView("false")) {
        mt->setEncryption(MailTransport::Transport::EnumEncryption::None);
    } else {
        qCDebug(BALSAPLUGIN_LOG) << " unknown ssl value :" << ssl;
    }

    // TODO const QString anonymous = grp.readEntry(QStringLiteral("Anonymous"));

    // TODO
    storeTransport(mt, /*( smtp == defaultSmtp )*/ true); // FIXME
    mHashSmtp.insert(smtp, QString::number(mt->id()));

    // TODO
    /*
    Server=localhost:25
    Anonymous=false
    RememberPasswd=false
    SSL=false
    TLSMode=1
    BigMessage=0
    */
}

void BalsaSettings::readGlobalSettings(const KConfig &config)
{
    if (config.hasGroup(QStringLiteral("Compose"))) {
        KConfigGroup compose = config.group(QStringLiteral("Compose"));
        if (compose.hasKey(QStringLiteral("QuoteString"))) {
            const QString quote = compose.readEntry(QStringLiteral("QuoteString"));
            if (!quote.isEmpty()) {
                addKmailConfig(QStringLiteral("TemplateParser"), QStringLiteral("QuoteString"), quote);
            }
        }
    }
    if (config.hasGroup(QStringLiteral("MessageDisplay"))) {
        KConfigGroup messageDisplay = config.group(QStringLiteral("MessageDisplay"));
#if 0
        if (messageDisplay.hasKey(QStringLiteral("WordWrap"))) {
            //bool wordWrap = messageDisplay.readEntry(QStringLiteral("WordWrap"), false);
            //Q_UNUSED(wordWrap)
            //TODO not implemented in kmail.
        }
        if (messageDisplay.hasKey(QStringLiteral("WordWrapLength"))) {
            const int wordWrapLength = messageDisplay.readEntry(QStringLiteral("WordWrapLength"), -1);
            Q_UNUSED(wordWrapLength)
            //TODO not implemented in kmail
        }
#endif
        if (messageDisplay.hasKey(QStringLiteral("DateFormat"))) {
            const QString dateFormat = messageDisplay.readEntry(QStringLiteral("DateFormat"));
            if (!dateFormat.isEmpty()) {
                addKmailConfig(QStringLiteral("General"), QStringLiteral("customDateFormat"), dateFormat);
            }
        }
    }

    if (config.hasGroup(QStringLiteral("Sending"))) {
        KConfigGroup sending = config.group(QStringLiteral("Sending"));
        if (sending.hasKey(QStringLiteral("WordWrap"))) {
            const bool wordWrap = sending.readEntry(QStringLiteral("WordWrap"), false);
            addKmailConfig(QStringLiteral("Composer"), QStringLiteral("word-wrap"), wordWrap);
        }
        if (sending.hasKey(QStringLiteral("break-at"))) {
            const int wordWrapLength = sending.readEntry(QStringLiteral("break-at"), -1);
            if (wordWrapLength != -1) {
                addKmailConfig(QStringLiteral("Composer"), QStringLiteral("break-at"), wordWrapLength);
            }
        }
    }
    if (config.hasGroup(QStringLiteral("Global"))) {
        KConfigGroup global = config.group(QStringLiteral("Global"));
        if (global.hasKey(QStringLiteral("EmptyTrash"))) {
            const bool emptyTrash = global.readEntry(QStringLiteral("EmptyTrash"), false);
            addKmailConfig(QStringLiteral("General"), QStringLiteral("empty-trash-on-exit"), emptyTrash);
        }
    }
    if (config.hasGroup(QStringLiteral("Spelling"))) {
        KConfigGroup spellChecking = config.group(QStringLiteral("Spelling"));
        if (spellChecking.hasKey(QStringLiteral("SpellCheckActive"))) {
            const bool active = spellChecking.readEntry(QStringLiteral("SpellCheckActive"), false);
            addKmailConfig(QStringLiteral("Spelling"), QStringLiteral("backgroundCheckerEnabled"), active);
            addKmailConfig(QStringLiteral("Spelling"), QStringLiteral("checkerEnabledByDefault"), active);
        }
        if (spellChecking.hasKey(QStringLiteral("SpellCheckLanguage"))) {
            const QString spellCheck = spellChecking.readEntry(QStringLiteral("defaultLanguage"));
            addKmailConfig(QStringLiteral("Spelling"), QStringLiteral("defaultLanguage"), spellCheck);
        }
    }
}
