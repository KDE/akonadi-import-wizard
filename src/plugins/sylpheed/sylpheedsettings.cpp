/*
   SPDX-FileCopyrightText: 2012-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "sylpheedsettings.h"
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
    const QString sylpheedrc = path + QLatin1String("/sylpheedrc");
    if (QFileInfo::exists(sylpheedrc)) {
        KConfig configCommon(sylpheedrc);
        if (configCommon.hasGroup(QLatin1String("Common"))) {
            KConfigGroup common = configCommon.group(QLatin1String("Common"));
            checkMailOnStartup = (common.readEntry("check_on_startup", 1) == 1);

            if (common.readEntry(QStringLiteral("autochk_newmail"), 1) == 1) {
                intervalCheckMail = common.readEntry(QStringLiteral("autochk_interval"), -1);
            }
            readGlobalSettings(common);
        }
    }
    KConfig config(filename);
    const QStringList accountList = config.groupList().filter(QRegularExpression(QStringLiteral("Account: \\d+")));
    const QStringList::const_iterator end(accountList.constEnd());
    for (QStringList::const_iterator it = accountList.constBegin(); it != end; ++it) {
        KConfigGroup group = config.group(*it);
        readAccount(group, checkMailOnStartup, intervalCheckMail);
        readIdentity(group);
    }
    const QString customheaderrc = path + QLatin1String("/customheaderrc");
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
        const QStringList lst = line.split(QLatin1Char(':'));
        if (lst.count() == 3) {
            QString str = lst.at(2);
            str.remove(0, 1);
            header.insert(lst.at(1), str);
        }
    }
    if (!header.isEmpty()) {
        const int oldValue = readKmailSettings(QStringLiteral("General"), QStringLiteral("mime-header-count"));
        int newValue = header.count();
        if (oldValue != -1) {
            newValue += oldValue;
        }
        addKmailConfig(QStringLiteral("General"), QStringLiteral("mime-header-count"), newValue);
        int currentHeader = (oldValue > 0) ? oldValue : 0;
        for (QMapIterator<QString, QString> it(header); it.hasNext();) {
            it.next();
            addComposerHeaderGroup(QStringLiteral("Mime #%1").arg(currentHeader), (it).key(), (it).value());
            ++currentHeader;
        }
    }
}

void SylpheedSettings::readGlobalSettings(const KConfigGroup &group)
{
    const bool showTrayIcon = (group.readEntry("show_trayicon", 0) == 1);
    addKmailConfig(QStringLiteral("General"), QStringLiteral("SystemTrayEnabled"), showTrayIcon);

    const bool cleanTrashOnExit = (group.readEntry("clean_trash_on_exit", 0) == 1);
    addKmailConfig(QStringLiteral("General"), QStringLiteral("empty-trash-on-exit"), cleanTrashOnExit);

    const bool alwaysMarkReadOnShowMsg = (group.readEntry("always_mark_read_on_show_msg", 0) == 1);
    if (alwaysMarkReadOnShowMsg) {
        addKmailConfig(QStringLiteral("Behaviour"), QStringLiteral("DelayedMarkAsRead"), true);
        addKmailConfig(QStringLiteral("Behaviour"), QStringLiteral("DelayedMarkTime"), 0);
    }

    if (group.readEntry("enable_autosave", 0) == 1) {
        const int autosaveInterval = group.readEntry("autosave_interval", 5);
        addKmailConfig(QStringLiteral("Composer"), QStringLiteral("autosave"), autosaveInterval);
    }
    const bool checkAttach = (group.readEntry("check_attach", 0) == 1);
    addKmailConfig(QStringLiteral("Composer"), QStringLiteral("showForgottenAttachmentWarning"), checkAttach);

    const QString attachStr = group.readEntry("check_attach_str");
    if (!attachStr.isEmpty()) {
        addKmailConfig(QStringLiteral("Composer"), QStringLiteral("attachment-keywords"), attachStr);
    }

    const int lineWrap = group.readEntry("linewrap_length", 80);
    addKmailConfig(QStringLiteral("Composer"), QStringLiteral("break-at"), lineWrap);
    addKmailConfig(QStringLiteral("Composer"), QStringLiteral("word-wrap"), true);

    if (group.readEntry(QStringLiteral("recycle_quote_colors"), 0) == 1) {
        addKmailConfig(QStringLiteral("Reader"), QStringLiteral("RecycleQuoteColors"), true);
    }

    if (group.readEntry(QStringLiteral("auto_signature"), 0) == 0) {
        addKmailConfig(QStringLiteral("Composer"), QStringLiteral("signature"), QStringLiteral("manual"));
    }

    if (group.readEntry(QStringLiteral("auto_ext_editor"), -1) == 1) {
        addKmailConfig(QStringLiteral("General"), QStringLiteral("use-external-editor"), true);

        const QString externalEditor = group.readEntry(QStringLiteral("mime_open_command"));
        if (!externalEditor.isEmpty()) {
            addKmailConfig(QStringLiteral("General"), QStringLiteral("external-editor"), externalEditor);
        }
    }

    readSettingsColor(group);
    readTemplateFormat(group);
    readTagColor(group);
    readDateFormat(group);
}

void SylpheedSettings::readTemplateFormat(const KConfigGroup &group)
{
    const QString replyQuote = group.readEntry(QStringLiteral("reply_quote_mark"));
    if (!replyQuote.isEmpty()) {
        addKmailConfig(QStringLiteral("TemplateParser"), QStringLiteral("QuoteString"), replyQuote);
    }
    const QString forwardQuote = group.readEntry(QStringLiteral("forward_quote_mark"));
    if (!forwardQuote.isEmpty()) {
        // Not implemented in kmail
    }
    const QString replyQuoteFormat = group.readEntry(QStringLiteral("reply_quote_format"));
    if (!replyQuoteFormat.isEmpty()) {
        addKmailConfig(QStringLiteral("TemplateParser"), QStringLiteral("TemplateReply"), convertToKmailTemplate(replyQuoteFormat));
    }
    const QString forwardQuoteFormat = group.readEntry(QStringLiteral("forward_quote_format"));
    if (!forwardQuoteFormat.isEmpty()) {
        addKmailConfig(QStringLiteral("TemplateParser"), QStringLiteral("TemplateForward"), convertToKmailTemplate(forwardQuoteFormat));
    }
}

void SylpheedSettings::readDateFormat(const KConfigGroup &group)
{
    const QString dateFormat = group.readEntry(QStringLiteral("date_format"));
    if (!dateFormat.isEmpty()) {
        addKmailConfig(QStringLiteral("General"), QStringLiteral("customDateFormat"), dateFormat);
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
    newTemplate.replace(QLatin1String("%date"), QLatin1String("%DATE"));
    newTemplate.replace(QLatin1String("%d"), QLatin1String("%DATE"));
    newTemplate.replace(QLatin1String("%from"), QLatin1String("%OTONAME"));
    newTemplate.replace(QLatin1String("%f"), QLatin1String("%OTONAME"));
    newTemplate.replace(QLatin1String("%to"), QLatin1String("%TONAME"));
    newTemplate.replace(QLatin1String("%t"), QLatin1String("%TONAME"));
    newTemplate.replace(QLatin1String("%cc"), QLatin1String("%CCNAME"));
    newTemplate.replace(QLatin1String("%c"), QLatin1String("%CCNAME"));

    newTemplate.replace(QLatin1String("%email"), QLatin1String("%OFROMNAME"));
    newTemplate.replace(QLatin1String("%A"), QLatin1String("%OFROMNAME"));

    newTemplate.replace(QLatin1String("%cursor"), QLatin1String("%CURSOR"));
    newTemplate.replace(QLatin1String("%X"), QLatin1String("%CURSOR"));

    newTemplate.replace(QLatin1String("%msg"), QLatin1String("%TEXT"));
    newTemplate.replace(QLatin1String("%M"), QLatin1String("%TEXT"));

    newTemplate.replace(QLatin1String("%quoted_msg"), QLatin1String("%QUOTE"));
    newTemplate.replace(QLatin1String("%Q"), QLatin1String("%QUOTE"));

    newTemplate.replace(QLatin1String("%subject"), QLatin1String("%OFULLSUBJECT"));
    newTemplate.replace(QLatin1String("%s"), QLatin1String("%OFULLSUBJECT"));

    newTemplate.replace(QLatin1String("%messageid"), QLatin1String("%MSGID"));
    newTemplate.replace(QLatin1String("%i"), QLatin1String("%MSGID"));

    newTemplate.replace(QLatin1String("%firstname"), QLatin1String("%OFROMNAME"));
    newTemplate.replace(QLatin1String("%F"), QLatin1String("%OFROMNAME"));

    newTemplate.replace(QLatin1String("%lastname"), QLatin1String("%OFROMLNAME"));
    newTemplate.replace(QLatin1String("%L"), QLatin1String("%OFROMLNAME"));

    newTemplate.replace(QLatin1String("%fullname"), QLatin1String("%OFROMFNAME"));
    newTemplate.replace(QLatin1String("%N"), QLatin1String("%OFROMFNAME"));
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
    settings.insert(QStringLiteral("Host"), host);

    const QString name = accountConfig.readEntry(QStringLiteral("name"));
    const QString inbox = MailCommon::Util::convertFolderPathToCollectionStr(accountConfig.readEntry(QStringLiteral("inbox")));
    settings.insert(QStringLiteral("TargetCollection"), inbox);
    int port = 0;
    if (SylpheedSettingsUtils::readConfig(QStringLiteral("pop_port"), accountConfig, port, true)) {
        settings.insert(QStringLiteral("Port"), port);
    }
    if (accountConfig.hasKey(QStringLiteral("ssl_pop"))) {
        const int sslPop = accountConfig.readEntry(QStringLiteral("ssl_pop"), 0);
        switch (sslPop) {
        case 0:
            // Nothing
            break;
        case 1:
            settings.insert(QStringLiteral("UseSSL"), true);
            break;
        case 2:
            settings.insert(QStringLiteral("UseTLS"), true);
            break;
        default:
            qCDebug(SYLPHEEDPLUGIN_LOG) << " unknown ssl_pop value " << sslPop;
            break;
        }
    }
    if (accountConfig.hasKey(QStringLiteral("remove_mail"))) {
        const bool removeMail = (accountConfig.readEntry(QStringLiteral("remove_mail"), 1) == 1);
        settings.insert(QStringLiteral("LeaveOnServer"), removeMail);
    }

    if (accountConfig.hasKey(QStringLiteral("message_leave_time"))) {
        settings.insert(QStringLiteral("LeaveOnServerDays"), accountConfig.readEntry(QStringLiteral("message_leave_time")));
    }
    const QString user = accountConfig.readEntry(QStringLiteral("user_id"));
    settings.insert(QStringLiteral("Login"), user);

    const QString password = accountConfig.readEntry(QStringLiteral("password"));
    settings.insert(QStringLiteral("Password"), password);

    // use_apop_auth
    if (accountConfig.hasKey(QStringLiteral("use_apop_auth"))) {
        const bool useApop = (accountConfig.readEntry(QStringLiteral("use_apop_auth"), 1) == 1);
        if (useApop) {
            settings.insert(QStringLiteral("AuthenticationMethod"), MailTransport::Transport::EnumAuthenticationType::APOP);
        }
    }
    if (intervalCheckMail != -1) {
        settings.insert(QStringLiteral("IntervalCheckEnabled"), true);
        settings.insert(QStringLiteral("IntervalCheckInterval"), intervalCheckMail);
    }

    const QString agentIdentifyName = LibImportWizard::AbstractBase::createResource(QStringLiteral("akonadi_pop3_resource"), name, settings);
    addCheckMailOnStartup(agentIdentifyName, checkMailOnStartup);
    const bool enableManualCheck = (accountConfig.readEntry(QStringLiteral("receive_at_get_all"), 0) == 1);
    addToManualCheck(agentIdentifyName, enableManualCheck);
}

void SylpheedSettings::readImapAccount(const KConfigGroup &accountConfig, bool checkMailOnStartup, int intervalCheckMail)
{
    QMap<QString, QVariant> settings;
    const QString host = accountConfig.readEntry("receive_server");
    settings.insert(QStringLiteral("ImapServer"), host);

    const QString name = accountConfig.readEntry(QStringLiteral("name"));
    const int sslimap = accountConfig.readEntry(QStringLiteral("ssl_imap"), 0);
    switch (sslimap) {
    case 0:
        // None
        settings.insert(QStringLiteral("Safety"), QStringLiteral("NONE"));
        break;
    case 1:
        // SSL
        settings.insert(QStringLiteral("Safety"), QStringLiteral("SSL"));
        break;
    case 2:
        // TLS
        settings.insert(QStringLiteral("Safety"), QStringLiteral("STARTTLS"));
        break;
    default:
        qCDebug(SYLPHEEDPLUGIN_LOG) << " sslimap unknown " << sslimap;
        break;
    }

    int port = 0;
    if (SylpheedSettingsUtils::readConfig(QStringLiteral("imap_port"), accountConfig, port, true)) {
        settings.insert(QStringLiteral("ImapPort"), port);
    }

    QString trashFolder;
    if (SylpheedSettingsUtils::readConfig(QStringLiteral("trash_folder"), accountConfig, trashFolder, false)) {
        settings.insert(QStringLiteral("TrashCollection"), MailCommon::Util::convertFolderPathToCollectionId(trashFolder));
    }

    const int auth = accountConfig.readEntry(QStringLiteral("imap_auth_method"), 0);
    switch (auth) {
    case 0:
        break;
    case 1: // Login
        settings.insert(QStringLiteral("Authentication"), MailTransport::Transport::EnumAuthenticationType::LOGIN);
        break;
    case 2: // Cram-md5
        settings.insert(QStringLiteral("Authentication"), MailTransport::Transport::EnumAuthenticationType::CRAM_MD5);
        break;
    case 4: // Plain
        settings.insert(QStringLiteral("Authentication"), MailTransport::Transport::EnumAuthenticationType::PLAIN);
        break;
    default:
        qCDebug(SYLPHEEDPLUGIN_LOG) << " imap auth unknown " << auth;
        break;
    }

    if (intervalCheckMail != -1) {
        settings.insert(QStringLiteral("IntervalCheckEnabled"), true);
        settings.insert(QStringLiteral("IntervalCheckTime"), intervalCheckMail);
    }

    const QString password = accountConfig.readEntry(QStringLiteral("password"));
    settings.insert(QStringLiteral("Password"), password);

    const QString agentIdentifyName = LibImportWizard::AbstractBase::createResource(QStringLiteral("akonadi_imap_resource"), name, settings);
    addCheckMailOnStartup(agentIdentifyName, checkMailOnStartup);

    const bool enableManualCheck = (accountConfig.readEntry(QStringLiteral("receive_at_get_all"), 0) == 1);
    addToManualCheck(agentIdentifyName, enableManualCheck);
}

void SylpheedSettings::readAccount(const KConfigGroup &accountConfig, bool checkMailOnStartup, int intervalCheckMail)
{
    if (accountConfig.hasKey(QStringLiteral("protocol"))) {
        const int protocol = accountConfig.readEntry(QStringLiteral("protocol"), 0);
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
    QString name = accountConfig.readEntry(QStringLiteral("name"));
    KIdentityManagementCore::Identity *identity = createIdentity(name);

    identity->setFullName(name);
    identity->setIdentityName(name);
    const QString organization = accountConfig.readEntry(QStringLiteral("organization"), QString());
    identity->setOrganization(organization);
    const QString email = accountConfig.readEntry(QStringLiteral("address"));
    identity->setPrimaryEmailAddress(email);

    QString value;
    if (SylpheedSettingsUtils::readConfig(QStringLiteral("auto_bcc"), accountConfig, value, true)) {
        identity->setBcc(value);
    }
    if (SylpheedSettingsUtils::readConfig(QStringLiteral("auto_cc"), accountConfig, value, true)) {
        identity->setCc(value);
    }
    if (SylpheedSettingsUtils::readConfig(QStringLiteral("auto_replyto"), accountConfig, value, true)) {
        identity->setReplyToAddr(value);
    }

    if (SylpheedSettingsUtils::readConfig(QStringLiteral("daft_folder"), accountConfig, value, false)) {
        identity->setDrafts(MailCommon::Util::convertFolderPathToCollectionStr(value));
    }

    if (SylpheedSettingsUtils::readConfig(QStringLiteral("sent_folder"), accountConfig, value, false)) {
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
        mt->setIdentifier(QStringLiteral("SMTP"));
        mt->setName(smtpserver);
        mt->setHost(smtpserver);
        int port = 0;
        if (SylpheedSettingsUtils::readConfig(QStringLiteral("smtp_port"), accountConfig, port, true)) {
            mt->setPort(port);
        }
        const QString user = accountConfig.readEntry(QStringLiteral("smtp_user_id"));

        if (!user.isEmpty()) {
            mt->setUserName(user);
            mt->setRequiresAuthentication(true);
        }
        const QString password = accountConfig.readEntry(QStringLiteral("smtp_password"));
        if (!password.isEmpty()) {
            mt->setStorePassword(true);
            mt->setPassword(password);
        }
        if (accountConfig.readEntry(QStringLiteral("use_smtp_auth"), 0) == 1) {
            const int authMethod = accountConfig.readEntry(QStringLiteral("smtp_auth_method"), 0);
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
        const int sslSmtp = accountConfig.readEntry(QStringLiteral("ssl_smtp"), 0);
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
        if (SylpheedSettingsUtils::readConfig(QStringLiteral("domain"), accountConfig, domainName, false)) {
            mt->setLocalHostname(domainName);
        }

        storeTransport(mt, true);
        return QString::number(mt->id());
    }
    return {};
}
