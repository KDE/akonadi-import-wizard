/*
   SPDX-FileCopyrightText: 2012-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "balsasettings.h"
using namespace Qt::Literals::StringLiterals;

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
    if (config.hasGroup(u"MailboxChecking"_s)) {
        KConfigGroup grp = config.group(u"MailboxChecking"_s);
        autoCheck = grp.readEntry(u"Auto"_s, false);
        autoCheckDelay = grp.readEntry(u"AutoDelay"_s, -1);
    }

    const QStringList mailBoxList = config.groupList().filter(QRegularExpression(u"mailbox-"_s));
    for (const QString &mailBox : mailBoxList) {
        KConfigGroup grp = config.group(mailBox);
        readAccount(grp, autoCheck, autoCheckDelay);
    }

    const QStringList smtpList = config.groupList().filter(QRegularExpression(u"smtp-server-"_s));
    for (const QString &smtp : smtpList) {
        KConfigGroup grp = config.group(smtp);
        readTransport(grp);
    }
    readGlobalSettings(config);
}

void BalsaSettings::readAccount(const KConfigGroup &grp, bool autoCheck, int autoDelay)
{
    Q_UNUSED(autoDelay)
    const QString type = grp.readEntry(u"Type"_s);
    bool check = grp.readEntry(u"Check"_s, false);
    if (type == QLatin1StringView("LibBalsaMailboxPOP3")) {
        QMap<QString, QVariant> settings;
        const QString server = grp.readEntry(u"Server"_s);
        settings.insert(u"Host"_s, server);
        const QString name = grp.readEntry(u"Name"_s);

        // const bool apop = grp.readEntry(u"DisableApop"_s, false);
        // Q_UNUSED(apop)
        const QString agentIdentifyName = LibImportWizard::AbstractBase::createResource(u"akonadi_pop3_resource"_s, name, settings);

        addCheckMailOnStartup(agentIdentifyName, autoCheck);
        addToManualCheck(agentIdentifyName, check);
    } else if (type == QLatin1StringView("LibBalsaMailboxImap")) {
        QMap<QString, QVariant> settings;
        const QString server = grp.readEntry(u"Server"_s);
        settings.insert(u"ImapServer"_s, server);
        const QString name = grp.readEntry(u"Name"_s);
        const QString agentIdentifyName = LibImportWizard::AbstractBase::createResource(u"akonadi_imap_resource"_s, name, settings);
        addCheckMailOnStartup(agentIdentifyName, autoCheck);
        addToManualCheck(agentIdentifyName, check);
    } else {
        qCDebug(BALSAPLUGIN_LOG) << " unknown account type :" << type;
    }
}

void BalsaSettings::readIdentity(const KConfigGroup &grp)
{
    QString name = grp.readEntry(u"FullName"_s);
    KIdentityManagementCore::Identity *newIdentity = createIdentity(name);
    newIdentity->setFullName(name);
    newIdentity->setPrimaryEmailAddress(grp.readEntry(u"Address"_s));
    newIdentity->setReplyToAddr(grp.readEntry(u"ReplyTo"_s));
    newIdentity->setBcc(grp.readEntry(u"Bcc"_s));
    const QString smtp = grp.readEntry(u"SmtpServer"_s);
    if (!smtp.isEmpty()) {
        const QString val = mHashSmtp.value(smtp);
        if (val.isEmpty()) {
            newIdentity->setTransport(val);
        }
    }

    const QString signaturePath = grp.readEntry(u"SignaturePath"_s);
    if (!signaturePath.isEmpty()) {
        KIdentityManagementCore::Signature signature;
        if (grp.readEntry(u"SigExecutable"_s, false)) {
            signature.setPath(signaturePath, true);
            signature.setType(KIdentityManagementCore::Signature::FromCommand);
        } else {
            signature.setType(KIdentityManagementCore::Signature::FromFile);
        }
        newIdentity->setSignature(signature);
    }

    const QString xfacePathStr = grp.readEntry(u"XFacePath"_s);
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
    const QString smtp = grp.name().remove(u"smtp-server-"_s);
    const QString server = grp.readEntry(u"Server"_s);
    mt->setHost(server);

    const int tlsMode = grp.readEntry(u"TLSMode"_s, -1);
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

    const QString ssl = grp.readEntry(u"SSL"_s);
    if (ssl == QLatin1StringView("true")) {
        mt->setEncryption(MailTransport::Transport::EnumEncryption::SSL);
    } else if (ssl == QLatin1StringView("false")) {
        mt->setEncryption(MailTransport::Transport::EnumEncryption::None);
    } else {
        qCDebug(BALSAPLUGIN_LOG) << " unknown ssl value :" << ssl;
    }

    // TODO const QString anonymous = grp.readEntry(u"Anonymous"_s);

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
    if (config.hasGroup(u"Compose"_s)) {
        KConfigGroup compose = config.group(u"Compose"_s);
        if (compose.hasKey(u"QuoteString"_s)) {
            const QString quote = compose.readEntry(u"QuoteString"_s);
            if (!quote.isEmpty()) {
                addKmailConfig(u"TemplateParser"_s, u"QuoteString"_s, quote);
            }
        }
    }
    if (config.hasGroup(u"MessageDisplay"_s)) {
        KConfigGroup messageDisplay = config.group(u"MessageDisplay"_s);
#if 0
        if (messageDisplay.hasKey(u"WordWrap"_s)) {
            //bool wordWrap = messageDisplay.readEntry(u"WordWrap"_s, false);
            //Q_UNUSED(wordWrap)
            //TODO not implemented in kmail.
        }
        if (messageDisplay.hasKey(u"WordWrapLength"_s)) {
            const int wordWrapLength = messageDisplay.readEntry(u"WordWrapLength"_s, -1);
            Q_UNUSED(wordWrapLength)
            //TODO not implemented in kmail
        }
#endif
        if (messageDisplay.hasKey(u"DateFormat"_s)) {
            const QString dateFormat = messageDisplay.readEntry(u"DateFormat"_s);
            if (!dateFormat.isEmpty()) {
                addKmailConfig(u"General"_s, u"customDateFormat"_s, dateFormat);
            }
        }
    }

    if (config.hasGroup(u"Sending"_s)) {
        KConfigGroup sending = config.group(u"Sending"_s);
        if (sending.hasKey(u"WordWrap"_s)) {
            const bool wordWrap = sending.readEntry(u"WordWrap"_s, false);
            addKmailConfig(u"Composer"_s, u"word-wrap"_s, wordWrap);
        }
        if (sending.hasKey(u"break-at"_s)) {
            const int wordWrapLength = sending.readEntry(u"break-at"_s, -1);
            if (wordWrapLength != -1) {
                addKmailConfig(u"Composer"_s, u"break-at"_s, wordWrapLength);
            }
        }
    }
    if (config.hasGroup(u"Global"_s)) {
        KConfigGroup global = config.group(u"Global"_s);
        if (global.hasKey(u"EmptyTrash"_s)) {
            const bool emptyTrash = global.readEntry(u"EmptyTrash"_s, false);
            addKmailConfig(u"General"_s, u"empty-trash-on-exit"_s, emptyTrash);
        }
    }
    if (config.hasGroup(u"Spelling"_s)) {
        KConfigGroup spellChecking = config.group(u"Spelling"_s);
        if (spellChecking.hasKey(u"SpellCheckActive"_s)) {
            const bool active = spellChecking.readEntry(u"SpellCheckActive"_s, false);
            addKmailConfig(u"Spelling"_s, u"backgroundCheckerEnabled"_s, active);
            addKmailConfig(u"Spelling"_s, u"checkerEnabledByDefault"_s, active);
        }
        if (spellChecking.hasKey(u"SpellCheckLanguage"_s)) {
            const QString spellCheck = spellChecking.readEntry(u"defaultLanguage"_s);
            addKmailConfig(u"Spelling"_s, u"defaultLanguage"_s, spellCheck);
        }
    }
}
