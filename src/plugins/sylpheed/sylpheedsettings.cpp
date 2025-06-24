/*
   SPDX-FileCopyrightText: 2012-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "sylpheedsettings.h"
using namespace Qt::Literals::StringLiterals;

#include "sylpheedsettingsutils.h"
#include <MailCommon/MailUtil>
#include <MailTransport/TransportManager>
#include <QFileInfo>

#include <KIdentityManagementCore/Identity>
#include <KIdentityManagementCore/Signature>

#include "sylpheedplugin_debug.h"
#include <KConfig>
#include <KConfigGroup>

#include <QFile>
#include <QRegularExpression>
#include <QStringList>

SylpheedSettings::SylpheedSettings() = default;

SylpheedSettings::~SylpheedSettings() = default;

void SylpheedSettings::importSettings(const QString &filename, const QString &path)
{
    bool checkMailOnStartup = true;
    int intervalCheckMail = -1;
    const QString sylpheedrc = path + QLatin1StringView("/sylpheedrc");
    if (QFileInfo::exists(sylpheedrc)) {
        KConfig configCommon(sylpheedrc);
        if (configCommon.hasGroup(QLatin1StringView("Common"))) {
            KConfigGroup common = configCommon.group(u"Common"_s);
            checkMailOnStartup = (common.readEntry("check_on_startup", 1) == 1);

            if (common.readEntry(u"autochk_newmail"_s, 1) == 1) {
                intervalCheckMail = common.readEntry(u"autochk_interval"_s, -1);
            }
            readGlobalSettings(common);
        }
    }
    KConfig config(filename);
    const QStringList accountList = config.groupList().filter(QRegularExpression(u"Account: \\d+"_s));
    const QStringList::const_iterator end(accountList.constEnd());
    for (QStringList::const_iterator it = accountList.constBegin(); it != end; ++it) {
        KConfigGroup group = config.group(*it);
        readAccount(group, checkMailOnStartup, intervalCheckMail);
        readIdentity(group);
    }
    const QString customheaderrc = path + QLatin1StringView("/customheaderrc");
    QFile customHeaderFile(customheaderrc);
    if (customHeaderFile.exists()) {
        if (!customHeaderFile.open(QIODevice::ReadOnly)) {
            qCDebug(SYLPHEEDPLUGIN_LOG) << " We can't open file" << customheaderrc;
        } else {
            readCustomHeader(&customHeaderFile);
        }
    }
}

void SylpheedSettings::readCustomHeader(QFile *customHeaderFile)
{
    // In sylpheed we define custom header from account.
    // In kmail it's global
    QTextStream stream(customHeaderFile);
    QMap<QString, QString> header;
    while (!stream.atEnd()) {
        const QString line = stream.readLine();
        const QStringList lst = line.split(u':');
        if (lst.count() == 3) {
            QString str = lst.at(2);
            str.remove(0, 1);
            header.insert(lst.at(1), str);
        }
    }
    if (!header.isEmpty()) {
        const int oldValue = readKmailSettings(u"General"_s, u"mime-header-count"_s);
        int newValue = header.count();
        if (oldValue != -1) {
            newValue += oldValue;
        }
        addKmailConfig(u"General"_s, u"mime-header-count"_s, newValue);
        int currentHeader = (oldValue > 0) ? oldValue : 0;
        for (QMapIterator<QString, QString> it(header); it.hasNext();) {
            it.next();
            addComposerHeaderGroup(u"Mime #%1"_s.arg(currentHeader), (it).key(), (it).value());
            ++currentHeader;
        }
    }
}

void SylpheedSettings::readGlobalSettings(const KConfigGroup &group)
{
    const bool showTrayIcon = (group.readEntry("show_trayicon", 0) == 1);
    addKmailConfig(u"General"_s, u"SystemTrayEnabled"_s, showTrayIcon);

    const bool cleanTrashOnExit = (group.readEntry("clean_trash_on_exit", 0) == 1);
    addKmailConfig(u"General"_s, u"empty-trash-on-exit"_s, cleanTrashOnExit);

    const bool alwaysMarkReadOnShowMsg = (group.readEntry("always_mark_read_on_show_msg", 0) == 1);
    if (alwaysMarkReadOnShowMsg) {
        addKmailConfig(u"Behaviour"_s, u"DelayedMarkAsRead"_s, true);
        addKmailConfig(u"Behaviour"_s, u"DelayedMarkTime"_s, 0);
    }

    if (group.readEntry("enable_autosave", 0) == 1) {
        const int autosaveInterval = group.readEntry("autosave_interval", 5);
        addKmailConfig(u"Composer"_s, u"autosave"_s, autosaveInterval);
    }
    const bool checkAttach = (group.readEntry("check_attach", 0) == 1);
    addKmailConfig(u"Composer"_s, u"showForgottenAttachmentWarning"_s, checkAttach);

    const QString attachStr = group.readEntry("check_attach_str");
    if (!attachStr.isEmpty()) {
        addKmailConfig(u"Composer"_s, u"attachment-keywords"_s, attachStr);
    }

    const int lineWrap = group.readEntry("linewrap_length", 80);
    addKmailConfig(u"Composer"_s, u"break-at"_s, lineWrap);
    addKmailConfig(u"Composer"_s, u"word-wrap"_s, true);

    if (group.readEntry(u"recycle_quote_colors"_s, 0) == 1) {
        addKmailConfig(u"Reader"_s, u"RecycleQuoteColors"_s, true);
    }

    if (group.readEntry(u"auto_signature"_s, 0) == 0) {
        addKmailConfig(u"Composer"_s, u"signature"_s, QStringLiteral("manual"));
    }

    if (group.readEntry(u"auto_ext_editor"_s, -1) == 1) {
        addKmailConfig(u"General"_s, u"use-external-editor"_s, true);

        const QString externalEditor = group.readEntry(u"mime_open_command"_s);
        if (!externalEditor.isEmpty()) {
            addKmailConfig(u"General"_s, u"external-editor"_s, externalEditor);
        }
    }

    readSettingsColor(group);
    readTemplateFormat(group);
    readTagColor(group);
    readDateFormat(group);
}

void SylpheedSettings::readTemplateFormat(const KConfigGroup &group)
{
    const QString replyQuote = group.readEntry(u"reply_quote_mark"_s);
    if (!replyQuote.isEmpty()) {
        addKmailConfig(u"TemplateParser"_s, u"QuoteString"_s, replyQuote);
    }
    const QString forwardQuote = group.readEntry(u"forward_quote_mark"_s);
    if (!forwardQuote.isEmpty()) {
        // Not implemented in kmail
    }
    const QString replyQuoteFormat = group.readEntry(u"reply_quote_format"_s);
    if (!replyQuoteFormat.isEmpty()) {
        addKmailConfig(u"TemplateParser"_s, u"TemplateReply"_s, convertToKmailTemplate(replyQuoteFormat));
    }
    const QString forwardQuoteFormat = group.readEntry(u"forward_quote_format"_s);
    if (!forwardQuoteFormat.isEmpty()) {
        addKmailConfig(u"TemplateParser"_s, u"TemplateForward"_s, convertToKmailTemplate(forwardQuoteFormat));
    }
}

void SylpheedSettings::readDateFormat(const KConfigGroup &group)
{
    const QString dateFormat = group.readEntry(u"date_format"_s);
    if (!dateFormat.isEmpty()) {
        addKmailConfig(u"General"_s, u"customDateFormat"_s, dateFormat);
    }
}

void SylpheedSettings::readTagColor(const KConfigGroup &group)
{
    Q_UNUSED(group)
    // TODO
}

void SylpheedSettings::readSettingsColor(const KConfigGroup &group)
{
    const bool enableColor = group.readEntry("enable_color", false);
    if (enableColor) {
        const int colorLevel1 = group.readEntry("quote_level1_color", -1);
        if (colorLevel1 != -1) {
            //[Reader]  QuotedText1
        }
        const int colorLevel2 = group.readEntry("quote_level2_color", -1);
        if (colorLevel2 != -1) {
            //[Reader]  QuotedText2
        }
        const int colorLevel3 = group.readEntry("quote_level3_color", -1);
        if (colorLevel3 != -1) {
            //[Reader]  QuotedText3
        }
    }
}

QString SylpheedSettings::convertToKmailTemplate(const QString &templateStr)
{
    QString newTemplate = templateStr;
    newTemplate.replace(QLatin1StringView("%date"), QLatin1StringView("%DATE"));
    newTemplate.replace(QLatin1StringView("%d"), QLatin1StringView("%DATE"));
    newTemplate.replace(QLatin1StringView("%from"), QLatin1StringView("%OTONAME"));
    newTemplate.replace(QLatin1StringView("%f"), QLatin1StringView("%OTONAME"));
    newTemplate.replace(QLatin1StringView("%to"), QLatin1StringView("%TONAME"));
    newTemplate.replace(QLatin1StringView("%t"), QLatin1StringView("%TONAME"));
    newTemplate.replace(QLatin1StringView("%cc"), QLatin1StringView("%CCNAME"));
    newTemplate.replace(QLatin1StringView("%c"), QLatin1StringView("%CCNAME"));

    newTemplate.replace(QLatin1StringView("%email"), QLatin1StringView("%OFROMNAME"));
    newTemplate.replace(QLatin1StringView("%A"), QLatin1StringView("%OFROMNAME"));

    newTemplate.replace(QLatin1StringView("%cursor"), QLatin1StringView("%CURSOR"));
    newTemplate.replace(QLatin1StringView("%X"), QLatin1StringView("%CURSOR"));

    newTemplate.replace(QLatin1StringView("%msg"), QLatin1StringView("%TEXT"));
    newTemplate.replace(QLatin1StringView("%M"), QLatin1StringView("%TEXT"));

    newTemplate.replace(QLatin1StringView("%quoted_msg"), QLatin1StringView("%QUOTE"));
    newTemplate.replace(QLatin1StringView("%Q"), QLatin1StringView("%QUOTE"));

    newTemplate.replace(QLatin1StringView("%subject"), QLatin1StringView("%OFULLSUBJECT"));
    newTemplate.replace(QLatin1StringView("%s"), QLatin1StringView("%OFULLSUBJECT"));

    newTemplate.replace(QLatin1StringView("%messageid"), QLatin1StringView("%MSGID"));
    newTemplate.replace(QLatin1StringView("%i"), QLatin1StringView("%MSGID"));

    newTemplate.replace(QLatin1StringView("%firstname"), QLatin1StringView("%OFROMNAME"));
    newTemplate.replace(QLatin1StringView("%F"), QLatin1StringView("%OFROMNAME"));

    newTemplate.replace(QLatin1StringView("%lastname"), QLatin1StringView("%OFROMLNAME"));
    newTemplate.replace(QLatin1StringView("%L"), QLatin1StringView("%OFROMLNAME"));

    newTemplate.replace(QLatin1StringView("%fullname"), QLatin1StringView("%OFROMFNAME"));
    newTemplate.replace(QLatin1StringView("%N"), QLatin1StringView("%OFROMFNAME"));
    // TODO add more variable
    return newTemplate;
}

void SylpheedSettings::readSignature(const KConfigGroup &accountConfig, KIdentityManagementCore::Identity *identity)
{
    KIdentityManagementCore::Signature signature;
    const int signatureType = accountConfig.readEntry("signature_type", 0);
    switch (signatureType) {
    case 0: // File
        signature.setType(KIdentityManagementCore::Signature::FromFile);
        signature.setPath(accountConfig.readEntry("signature_path"), false);
        break;
    case 1: // Output
        signature.setType(KIdentityManagementCore::Signature::FromCommand);
        signature.setPath(accountConfig.readEntry("signature_path"), true);
        break;
    case 2: // Text
        signature.setType(KIdentityManagementCore::Signature::Inlined);
        signature.setText(accountConfig.readEntry("signature_text"));
        break;
    default:
        qCDebug(SYLPHEEDPLUGIN_LOG) << " signature type unknown :" << signatureType;
        break;
    }
    const int signatureEnabled = accountConfig.readEntry("auto_signature", -1);
    switch (signatureEnabled) {
    case -1:
        break;
    case 0:
        signature.setEnabledSignature(false);
        break;
    case 1:
        signature.setEnabledSignature(true);
        break;
    default:
        qCDebug(SYLPHEEDPLUGIN_LOG) << " auto_signature undefined " << signatureEnabled;
        break;
    }

    // TODO  const bool signatureBeforeQuote = ( accountConfig.readEntry( "signature_before_quote", 0 ) == 1 ); not implemented in kmail

    identity->setSignature(signature);
}

void SylpheedSettings::readPop3Account(const KConfigGroup &accountConfig, bool checkMailOnStartup, int intervalCheckMail)
{
    QMap<QString, QVariant> settings;
    const QString host = accountConfig.readEntry("receive_server");
    settings.insert(u"Host"_s, host);

    const QString name = accountConfig.readEntry(u"name"_s);
    const QString inbox = MailCommon::Util::convertFolderPathToCollectionStr(accountConfig.readEntry(u"inbox"_s));
    settings.insert(u"TargetCollection"_s, inbox);
    int port = 0;
    if (SylpheedSettingsUtils::readConfig(u"pop_port"_s, accountConfig, port, true)) {
        settings.insert(u"Port"_s, port);
    }
    if (accountConfig.hasKey(u"ssl_pop"_s)) {
        const int sslPop = accountConfig.readEntry(u"ssl_pop"_s, 0);
        switch (sslPop) {
        case 0:
            // Nothing
            break;
        case 1:
            settings.insert(u"UseSSL"_s, true);
            break;
        case 2:
            settings.insert(u"UseTLS"_s, true);
            break;
        default:
            qCDebug(SYLPHEEDPLUGIN_LOG) << " unknown ssl_pop value " << sslPop;
            break;
        }
    }
    if (accountConfig.hasKey(u"remove_mail"_s)) {
        const bool removeMail = (accountConfig.readEntry(u"remove_mail"_s, 1) == 1);
        settings.insert(u"LeaveOnServer"_s, removeMail);
    }

    if (accountConfig.hasKey(u"message_leave_time"_s)) {
        settings.insert(u"LeaveOnServerDays"_s, accountConfig.readEntry(u"message_leave_time"_s));
    }
    const QString user = accountConfig.readEntry(u"user_id"_s);
    settings.insert(u"Login"_s, user);

    const QString password = accountConfig.readEntry(u"password"_s);
    settings.insert(u"Password"_s, password);

    // use_apop_auth
    if (accountConfig.hasKey(u"use_apop_auth"_s)) {
        const bool useApop = (accountConfig.readEntry(u"use_apop_auth"_s, 1) == 1);
        if (useApop) {
            settings.insert(u"AuthenticationMethod"_s, MailTransport::Transport::EnumAuthenticationType::APOP);
        }
    }
    if (intervalCheckMail != -1) {
        settings.insert(u"IntervalCheckEnabled"_s, true);
        settings.insert(u"IntervalCheckInterval"_s, intervalCheckMail);
    }

    const QString agentIdentifyName = LibImportWizard::AbstractBase::createResource(u"akonadi_pop3_resource"_s, name, settings);
    addCheckMailOnStartup(agentIdentifyName, checkMailOnStartup);
    const bool enableManualCheck = (accountConfig.readEntry(u"receive_at_get_all"_s, 0) == 1);
    addToManualCheck(agentIdentifyName, enableManualCheck);
}

void SylpheedSettings::readImapAccount(const KConfigGroup &accountConfig, bool checkMailOnStartup, int intervalCheckMail)
{
    QMap<QString, QVariant> settings;
    const QString host = accountConfig.readEntry("receive_server");
    settings.insert(u"ImapServer"_s, host);

    const QString name = accountConfig.readEntry(u"name"_s);
    const int sslimap = accountConfig.readEntry(u"ssl_imap"_s, 0);
    switch (sslimap) {
    case 0:
        // None
        settings.insert(u"Safety"_s, u"NONE"_s);
        break;
    case 1:
        // SSL
        settings.insert(u"Safety"_s, u"SSL"_s);
        break;
    case 2:
        // TLS
        settings.insert(u"Safety"_s, u"STARTTLS"_s);
        break;
    default:
        qCDebug(SYLPHEEDPLUGIN_LOG) << " sslimap unknown " << sslimap;
        break;
    }

    int port = 0;
    if (SylpheedSettingsUtils::readConfig(u"imap_port"_s, accountConfig, port, true)) {
        settings.insert(u"ImapPort"_s, port);
    }

    QString trashFolder;
    if (SylpheedSettingsUtils::readConfig(u"trash_folder"_s, accountConfig, trashFolder, false)) {
        settings.insert(u"TrashCollection"_s, MailCommon::Util::convertFolderPathToCollectionId(trashFolder));
    }

    const int auth = accountConfig.readEntry(u"imap_auth_method"_s, 0);
    switch (auth) {
    case 0:
        break;
    case 1: // Login
        settings.insert(u"Authentication"_s, MailTransport::Transport::EnumAuthenticationType::LOGIN);
        break;
    case 2: // Cram-md5
        settings.insert(u"Authentication"_s, MailTransport::Transport::EnumAuthenticationType::CRAM_MD5);
        break;
    case 4: // Plain
        settings.insert(u"Authentication"_s, MailTransport::Transport::EnumAuthenticationType::PLAIN);
        break;
    default:
        qCDebug(SYLPHEEDPLUGIN_LOG) << " imap auth unknown " << auth;
        break;
    }

    if (intervalCheckMail != -1) {
        settings.insert(u"IntervalCheckEnabled"_s, true);
        settings.insert(u"IntervalCheckTime"_s, intervalCheckMail);
    }

    const QString password = accountConfig.readEntry(u"password"_s);
    settings.insert(u"Password"_s, password);

    const QString agentIdentifyName = LibImportWizard::AbstractBase::createResource(u"akonadi_imap_resource"_s, name, settings);
    addCheckMailOnStartup(agentIdentifyName, checkMailOnStartup);

    const bool enableManualCheck = (accountConfig.readEntry(u"receive_at_get_all"_s, 0) == 1);
    addToManualCheck(agentIdentifyName, enableManualCheck);
}

void SylpheedSettings::readAccount(const KConfigGroup &accountConfig, bool checkMailOnStartup, int intervalCheckMail)
{
    if (accountConfig.hasKey(u"protocol"_s)) {
        const int protocol = accountConfig.readEntry(u"protocol"_s, 0);
        switch (protocol) {
        case 0:
            readPop3Account(accountConfig, checkMailOnStartup, intervalCheckMail);
            break;
        case 3:
            // imap
            readImapAccount(accountConfig, checkMailOnStartup, intervalCheckMail);
            break;
        case 4:
            qCDebug(SYLPHEEDPLUGIN_LOG) << " Add it when nntp resource will implemented";
            // news
            break;
        case 5:
            // local
            break;
        default:
            qCDebug(SYLPHEEDPLUGIN_LOG) << " protocol not defined" << protocol;
            break;
        }
    }
}

void SylpheedSettings::readIdentity(const KConfigGroup &accountConfig)
{
    QString name = accountConfig.readEntry(u"name"_s);
    KIdentityManagementCore::Identity *identity = createIdentity(name);

    identity->setFullName(name);
    identity->setIdentityName(name);
    const QString organization = accountConfig.readEntry(u"organization"_s, QString());
    identity->setOrganization(organization);
    const QString email = accountConfig.readEntry(u"address"_s);
    identity->setPrimaryEmailAddress(email);

    QString value;
    if (SylpheedSettingsUtils::readConfig(u"auto_bcc"_s, accountConfig, value, true)) {
        identity->setBcc(value);
    }
    if (SylpheedSettingsUtils::readConfig(u"auto_cc"_s, accountConfig, value, true)) {
        identity->setCc(value);
    }
    if (SylpheedSettingsUtils::readConfig(u"auto_replyto"_s, accountConfig, value, true)) {
        identity->setReplyToAddr(value);
    }

    if (SylpheedSettingsUtils::readConfig(u"daft_folder"_s, accountConfig, value, false)) {
        identity->setDrafts(MailCommon::Util::convertFolderPathToCollectionStr(value));
    }

    if (SylpheedSettingsUtils::readConfig(u"sent_folder"_s, accountConfig, value, false)) {
        identity->setFcc(MailCommon::Util::convertFolderPathToCollectionStr(value));
    }

    const QString transportId = readTransport(accountConfig);
    if (!transportId.isEmpty()) {
        identity->setTransport(transportId);
    }
    readSignature(accountConfig, identity);
    storeIdentity(identity);
}

QString SylpheedSettings::readTransport(const KConfigGroup &accountConfig)
{
    const QString smtpserver = accountConfig.readEntry("smtp_server");

    if (!smtpserver.isEmpty()) {
        MailTransport::Transport *mt = createTransport();
        mt->setIdentifier(u"SMTP"_s);
        mt->setName(smtpserver);
        mt->setHost(smtpserver);
        int port = 0;
        if (SylpheedSettingsUtils::readConfig(u"smtp_port"_s, accountConfig, port, true)) {
            mt->setPort(port);
        }
        const QString user = accountConfig.readEntry(u"smtp_user_id"_s);

        if (!user.isEmpty()) {
            mt->setUserName(user);
            mt->setRequiresAuthentication(true);
        }
        const QString password = accountConfig.readEntry(u"smtp_password"_s);
        if (!password.isEmpty()) {
            mt->setStorePassword(true);
            mt->setPassword(password);
        }
        if (accountConfig.readEntry(u"use_smtp_auth"_s, 0) == 1) {
            const int authMethod = accountConfig.readEntry(u"smtp_auth_method"_s, 0);
            switch (authMethod) {
            case 0: // Automatic:
                mt->setAuthenticationType(MailTransport::Transport::EnumAuthenticationType::PLAIN); //????
                break;
            case 1: // Login
                mt->setAuthenticationType(MailTransport::Transport::EnumAuthenticationType::LOGIN);
                break;
            case 2: // Cram-MD5
                mt->setAuthenticationType(MailTransport::Transport::EnumAuthenticationType::CRAM_MD5);
                break;
            case 8: // Plain
                mt->setAuthenticationType(MailTransport::Transport::EnumAuthenticationType::PLAIN);
                break;
            default:
                qCDebug(SYLPHEEDPLUGIN_LOG) << " smtp authentication unknown :" << authMethod;
                break;
            }
        }
        const int sslSmtp = accountConfig.readEntry(u"ssl_smtp"_s, 0);
        switch (sslSmtp) {
        case 0:
            mt->setEncryption(MailTransport::Transport::EnumEncryption::None);
            break;
        case 1:
            mt->setEncryption(MailTransport::Transport::EnumEncryption::SSL);
            break;
        case 2:
            mt->setEncryption(MailTransport::Transport::EnumEncryption::TLS);
            break;
        default:
            qCDebug(SYLPHEEDPLUGIN_LOG) << " smtp ssl config unknown :" << sslSmtp;
            break;
        }
        QString domainName;
        if (SylpheedSettingsUtils::readConfig(u"domain"_s, accountConfig, domainName, false)) {
            mt->setLocalHostname(domainName);
        }

        storeTransport(mt, true);
        return QString::number(mt->id());
    }
    return {};
}
