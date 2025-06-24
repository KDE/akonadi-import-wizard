/*
   SPDX-FileCopyrightText: 2012-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "thunderbirdsettings.h"
using namespace Qt::Literals::StringLiterals;

#include <MailCommon/MailUtil>
#include <MailTransport/TransportManager>

#include <KIdentityManagementCore/Identity>
#include <KIdentityManagementCore/Signature>

#include "thunderbirdplugin_debug.h"
#include <KContacts/VCardConverter>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QStandardPaths>
#include <QTextStream>
#include <QUrl>

ThunderbirdSettings::ThunderbirdSettings(const QString &filename)
    : mFileName(filename)
{
}

ThunderbirdSettings::~ThunderbirdSettings() = default;

void ThunderbirdSettings::importSettings()
{
    QFile file(mFileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qCDebug(THUNDERBIRDPLUGIN_LOG) << " We can't open file" << mFileName;
        return;
    }
    QTextStream stream(&file);
    while (!stream.atEnd()) {
        const QString line = stream.readLine();
        if (line.startsWith(QLatin1StringView("user_pref"))) {
            if (line.contains(QLatin1StringView("mail.smtpserver")) || line.contains(QLatin1StringView("mail.server."))
                || line.contains(QLatin1StringView("mail.identity.")) || line.contains(QLatin1StringView("mail.account."))
                || line.contains(QLatin1StringView("mail.accountmanager.")) || line.contains(QLatin1StringView("mailnews."))
                || line.contains(QLatin1StringView("mail.compose.")) || line.contains(QLatin1StringView("mail.spellcheck"))
                || line.contains(QLatin1StringView("mail.SpellCheckBeforeSend")) || line.contains(QLatin1StringView("spellchecker.dictionary"))
                || line.contains(QLatin1StringView("ldap_")) || line.contains(QLatin1StringView("mail.biff."))
                || line.contains(QLatin1StringView("mailnews.tags.")) || line.contains(QLatin1StringView("extensions.AutoResizeImage."))
                || line.contains(QLatin1StringView("mail.phishing.")) || line.contains(QLatin1StringView("mail.display_glyph"))
                || line.contains(QLatin1StringView("extensions.sieve.account."))) {
                insertIntoMap(line);
            }
        } else {
            if (!line.startsWith(u'#') || line.isEmpty() || line.startsWith(QLatin1StringView("/*")) || line.startsWith(QLatin1StringView(" */"))
                || line.startsWith(QLatin1StringView(" *"))) {
                qCDebug(THUNDERBIRDPLUGIN_LOG) << " unstored line :" << line;
            }
        }
    }
    const QString mailAccountPreference = mHashConfig.value(u"mail.accountmanager.accounts"_s).toString();
    if (mailAccountPreference.isEmpty()) {
        return;
    }
    mAccountList = mailAccountPreference.split(u',');
    readTransport();
    readAccount();
    readGlobalSettings();
    readLdapSettings();
    readTagSettings();
    readExtensionsSettings();
}

void ThunderbirdSettings::readExtensionsSettings()
{
    // AutoResizeImage
    const QString filterPatternEnabledStr = u"extensions.AutoResizeImage.filterPatterns"_s;
    if (mHashConfig.contains(filterPatternEnabledStr)) {
        const int filterPatternType = mHashConfig.value(filterPatternEnabledStr).toInt();
        addKmailConfig(u"AutoResizeImage"_s, u"filter-source-type"_s, filterPatternType);
    }
    const QString filterPatternListStr = u"extensions.AutoResizeImage.filteringPatternsList"_s;
    if (mHashConfig.contains(filterPatternListStr)) {
        const QString filterPatternList = mHashConfig.value(filterPatternListStr).toString();
        // TODO decode it.
        addKmailConfig(u"AutoResizeImage"_s, u"filter-source-pattern"_s, filterPatternList);
    }

    const QString enlargeImagesStr = u"extensions.AutoResizeImage.enlargeImages"_s;
    if (mHashConfig.contains(enlargeImagesStr)) {
        const bool enlargeImages = mHashConfig.value(enlargeImagesStr).toBool();
        addKmailConfig(u"AutoResizeImage"_s, u"enlarge-image-to-minimum"_s, enlargeImages);
    }

    const QString maxResolutionXStr = u"extensions.AutoResizeImage.maxResolutionX"_s;
    if (mHashConfig.contains(maxResolutionXStr)) {
        const int val = mHashConfig.value(maxResolutionXStr).toInt();
        int adaptedValue = adaptAutoResizeResolution(val, u"extensions.AutoResizeImage.maxResolutionXList"_s);
        addKmailConfig(u"AutoResizeImage"_s, u"maximum-width"_s, adaptedValue);
    }
    const QString maxResolutionYStr = u"extensions.AutoResizeImage.maxResolutionY"_s;
    if (mHashConfig.contains(maxResolutionYStr)) {
        const int val = mHashConfig.value(maxResolutionYStr).toInt();
        int adaptedValue = adaptAutoResizeResolution(val, u"extensions.AutoResizeImage.maxResolutionYList"_s);
        addKmailConfig(u"AutoResizeImage"_s, u"maximum-height"_s, adaptedValue);
    }
    const QString minResolutionXStr = u"extensions.AutoResizeImage.minResolutionX"_s;
    if (mHashConfig.contains(minResolutionXStr)) {
        const int val = mHashConfig.value(minResolutionXStr).toInt();
        int adaptedValue = adaptAutoResizeResolution(val, u"extensions.AutoResizeImage.minResolutionXList"_s);
        addKmailConfig(u"AutoResizeImage"_s, u"minimum-width"_s, adaptedValue);
    }
    const QString minResolutionYStr = u"extensions.AutoResizeImage.minResolutionY"_s;
    if (mHashConfig.contains(minResolutionYStr)) {
        const int val = mHashConfig.value(minResolutionYStr).toInt();
        int adaptedValue = adaptAutoResizeResolution(val, u"extensions.AutoResizeImage.minResolutionYList"_s);
        addKmailConfig(u"AutoResizeImage"_s, u"minimum-height"_s, adaptedValue);
    }

    // Default is true.
    const QString reduceImageStr(u"extensions.AutoResizeImage.reduceImages"_s);
    if (mHashConfig.contains(reduceImageStr)) {
        const bool reduce = mHashConfig.value(reduceImageStr).toBool();
        addKmailConfig(u"AutoResizeImage"_s, u"reduce-image-to-maximum"_s, reduce);
    } else {
        addKmailConfig(u"AutoResizeImage"_s, u"reduce-image-to-maximum"_s, false);
    }

    const QString filterMinimumStr(u"extensions.AutoResizeImage.filterMinimumSize"_s);
    if (mHashConfig.contains(filterMinimumStr)) {
        const bool filterMinimum = mHashConfig.value(filterMinimumStr).toBool();
        addKmailConfig(u"AutoResizeImage"_s, u"skip-image-lower-size-enabled"_s, filterMinimum);
    }
    const QString skipMinimumSizeStr(u"extensions.AutoResizeImage.minimumSize"_s);
    if (mHashConfig.contains(skipMinimumSizeStr)) {
        const int skipMinimumSize = mHashConfig.value(skipMinimumSizeStr).toInt();
        addKmailConfig(u"AutoResizeImage"_s, u"skip-image-lower-size"_s, skipMinimumSize);
    }
    const QString confirmBeforeResizingStr(u"extensions.AutoResizeImage.confirmResizing"_s);
    if (mHashConfig.contains(confirmBeforeResizingStr)) {
        const bool confirmBeforeResizing = mHashConfig.value(confirmBeforeResizingStr).toBool();
        addKmailConfig(u"AutoResizeImage"_s, u"ask-before-resizing"_s, confirmBeforeResizing);
    }
    // extensions.AutoResizeImage.convertImages : not implemented in kmail

    const QString conversionFormatStr(u"extensions.AutoResizeImage.conversionFormat"_s);
    if (mHashConfig.contains(conversionFormatStr)) {
        QString conversionFormat = mHashConfig.value(conversionFormatStr).toString();
        if (conversionFormat == QLatin1StringView("png")) {
            conversionFormat = u"PNG"_s;
        } else {
            conversionFormat = u"JPG"_s;
        }
        addKmailConfig(u"AutoResizeImage"_s, u"write-format"_s, conversionFormat);
    }

    const QString filterRecipientsStr(u"extensions.AutoResizeImage.filterRecipients"_s);
    if (mHashConfig.contains(filterRecipientsStr)) {
        const int filterRecipients = mHashConfig.value(filterRecipientsStr).toInt();
        switch (filterRecipients) {
        case 0:
            addKmailConfig(u"AutoResizeImage"_s, u"FilterRecipientType"_s, QStringLiteral("NoFilter"));
            break;
        case 1:
            addKmailConfig(u"AutoResizeImage"_s, u"FilterRecipientType"_s, QStringLiteral("ResizeEachEmailsContainsPattern"));
            break;
        case 2:
            addKmailConfig(u"AutoResizeImage"_s, u"FilterRecipientType"_s, QStringLiteral("ResizeOneEmailContainsPattern"));
            break;
        case 3:
            addKmailConfig(u"AutoResizeImage"_s, u"FilterRecipientType"_s, QStringLiteral("DontResizeEachEmailsContainsPattern"));
            break;
        case 4:
            addKmailConfig(u"AutoResizeImage"_s, u"FilterRecipientType"_s, QStringLiteral("DontResizeOneEmailContainsPattern"));
            break;
        default:
            qCDebug(THUNDERBIRDPLUGIN_LOG) << " unknown FilterRecipientType: " << filterRecipients;
            break;
        }
    }

    const QString filteringRecipientsPatternsWhiteListStr(u"extensions.AutoResizeImage.filteringRecipientsPatternsWhiteList"_s);
    if (mHashConfig.contains(filteringRecipientsPatternsWhiteListStr)) {
        const QString filteringRecipientsPatternsWhiteList = mHashConfig.value(filteringRecipientsPatternsWhiteListStr).toString();
        addKmailConfig(u"AutoResizeImage"_s, u"resize-emails-pattern"_s, filteringRecipientsPatternsWhiteList);
    }

    const QString filteringRecipientsPatternsBlackListStr(u"extensions.AutoResizeImage.filteringRecipientsPatternsBlackList"_s);
    if (mHashConfig.contains(filteringRecipientsPatternsBlackListStr)) {
        const QString filteringRecipientsPatternsBlackList = mHashConfig.value(filteringRecipientsPatternsBlackListStr).toString();
        addKmailConfig(u"AutoResizeImage"_s, u"do-not-resize-emails-pattern"_s, filteringRecipientsPatternsBlackList);
    }

    const QString filteringRenamingPatternStr(u"extensions.AutoResizeImage.renamingPattern"_s);
    if (mHashConfig.contains(filteringRenamingPatternStr)) {
        QString filteringRenamingPattern = mHashConfig.value(filteringRenamingPatternStr).toString();
        filteringRenamingPattern.replace(QLatin1StringView("%3Fn"), QLatin1StringView("%n"));
        filteringRenamingPattern.replace(QLatin1StringView("%3Ft"), QLatin1StringView("%t"));
        filteringRenamingPattern.replace(QLatin1StringView("%3Fd"), QLatin1StringView("%d"));
        filteringRenamingPattern.replace(QLatin1StringView("%3Fe"), QLatin1StringView("%e"));
        addKmailConfig(u"AutoResizeImage"_s, u"rename-resized-images-pattern"_s, filteringRenamingPattern);
    }

    const QString filteringRenamingImageStr(u"extensions.AutoResizeImage.renameResizedImages"_s);
    if (mHashConfig.contains(filteringRenamingImageStr)) {
        addKmailConfig(u"AutoResizeImage"_s, u"rename-resized-images"_s, true);
    }

    const QString filteringImageFormatsStr(u"extensions.AutoResizeImage.imageFormats"_s);
    if (mHashConfig.contains(filteringImageFormatsStr)) {
        const QString filteringImageFormats = mHashConfig.value(filteringImageFormatsStr).toString();
        // convert it.
        addKmailConfig(u"AutoResizeImage"_s, u"resize-image-with-formats-type"_s, filteringImageFormats);
    }

    const QString filteringImageFormatsEnabledStr(u"extensions.AutoResizeImage.filterFormats"_s);
    if (mHashConfig.contains(filteringImageFormatsEnabledStr)) {
        const bool filteringImageFormatsEnabled = mHashConfig.value(filteringImageFormatsEnabledStr).toBool();
        addKmailConfig(u"AutoResizeImage"_s, u"resize-image-with-formats"_s, filteringImageFormatsEnabled);
    }
}

int ThunderbirdSettings::adaptAutoResizeResolution(int index, const QString &configStrList)
{
    switch (index) {
    case 0:
        return 240;
    case 1:
        return 320;
    case 2:
        return 512;
    case 3:
        return 640;
    case 4:
        return 800;
    case 5:
        return 1024;
    case 6:
        return 1280;
    case 7:
        return 2048;
    case 8:
        return 1024;
    case 9: { // custom case
        if (mHashConfig.contains(configStrList)) {
            const QString res = mHashConfig.value(configStrList).toString();
            const QStringList lst = res.split(u';');
            int val = lst.last().toInt();
            return val;
        }
        [[fallthrough]];
    }
    default:
        return -1;
    }
}

void ThunderbirdSettings::readTagSettings()
{
    QList<tagStruct> v;
    v.reserve(mHashTag.size());
    for (const tagStruct &tag : std::as_const(mHashTag)) {
        v.push_back(tag);
    }

    ImportWizardUtil::addAkonadiTag(v);
}

void ThunderbirdSettings::readLdapSettings()
{
    // qCDebug(THUNDERBIRDPLUGIN_LOG)<<" mLdapAccountList:"<<mLdapAccountList;
    for (const QString &ldapAccountName : std::as_const(mLdapAccountList)) {
        ldapStruct ldap;
        const QString ldapDescription = u"%1.description"_s.arg(ldapAccountName);
        if (mHashConfig.contains(ldapDescription)) {
            ldap.description = mHashConfig.value(ldapDescription).toString();
        }
        const QString ldapAuthDn = u"%1.auth.dn"_s.arg(ldapAccountName);
        if (mHashConfig.contains(ldapAuthDn)) {
            ldap.dn = mHashConfig.value(ldapAuthDn).toString();
        }
        const QString ldapAuthSaslMech = u"%1.auth.saslmech"_s.arg(ldapAccountName);
        if (mHashConfig.contains(ldapAuthSaslMech)) {
            ldap.saslMech = mHashConfig.value(ldapAuthSaslMech).toString();
        }
        const QString ldapFilename = u"%1.filename"_s.arg(ldapAccountName);
        if (mHashConfig.contains(ldapFilename)) {
            ldap.fileName = mHashConfig.value(ldapFilename).toString();
        }
        const QString ldapMaxHits = u"%1.maxHits"_s.arg(ldapAccountName);
        if (mHashConfig.contains(ldapMaxHits)) {
            ldap.maxHint = mHashConfig.value(ldapMaxHits).toInt();
        }
        const QString ldapUri = u"%1.uri"_s.arg(ldapAccountName);
        if (mHashConfig.contains(ldapUri)) {
            ldap.ldapUrl = QUrl(mHashConfig.value(ldapUri).toString());
            ldap.port = ldap.ldapUrl.port();

            if (ldap.ldapUrl.scheme() == QLatin1StringView("ldaps")) {
                ldap.useSSL = true;
            } else if (ldap.ldapUrl.scheme() == QLatin1StringView("ldap")) {
                ldap.useSSL = false;
            } else {
                qCDebug(THUNDERBIRDPLUGIN_LOG) << " Security not implemented :" << ldap.ldapUrl.scheme();
            }
        }
        ImportWizardUtil::mergeLdap(ldap);
    }
}

void ThunderbirdSettings::readGlobalSettings()
{
    const QString markMessageReadStr = u"mailnews.mark_message_read.delay"_s;
    if (mHashConfig.contains(markMessageReadStr)) {
        const bool markMessageRead = mHashConfig.value(markMessageReadStr).toBool();
        addKmailConfig(u"Behaviour"_s, u"DelayedMarkAsRead"_s, markMessageRead);
    } else {
        // Default value
        addKmailConfig(u"Behaviour"_s, u"DelayedMarkAsRead"_s, true);
    }
    const QString markMessageReadIntervalStr = u"mailnews.mark_message_read.delay.interval"_s;
    if (mHashConfig.contains(markMessageReadIntervalStr)) {
        bool found = false;
        const int markMessageReadInterval = mHashConfig.value(markMessageReadIntervalStr).toInt(&found);
        if (found) {
            addKmailConfig(u"Behaviour"_s, u"DelayedMarkTime"_s, markMessageReadInterval);
        }
    } else {
        // Default 5 seconds
        addKmailConfig(u"Behaviour"_s, u"DelayedMarkTime"_s, 5);
    }

    const QString mailComposeAttachmentReminderStr = u"mail.compose.attachment_reminder"_s;
    if (mHashConfig.contains(mailComposeAttachmentReminderStr)) {
        const bool mailComposeAttachmentReminder = mHashConfig.value(mailComposeAttachmentReminderStr).toBool();
        addKmailConfig(u"Composer"_s, u"showForgottenAttachmentWarning"_s, mailComposeAttachmentReminder);
    } else {
        addKmailConfig(u"Composer"_s, u"showForgottenAttachmentWarning"_s, true);
    }

    const QString mailComposeAttachmentReminderKeywordsStr = u"mail.compose.attachment_reminder_keywords"_s;
    if (mHashConfig.contains(mailComposeAttachmentReminderKeywordsStr)) {
        const QString mailComposeAttachmentReminderKeywords = mHashConfig.value(mailComposeAttachmentReminderKeywordsStr).toString();
        addKmailConfig(u"Composer"_s, u"attachment-keywords"_s, mailComposeAttachmentReminderKeywords);
    } // not default value keep kmail use one default value

    const QString mailComposeAutosaveStr = u"mail.compose.autosave"_s;
    if (mHashConfig.contains(mailComposeAutosaveStr)) {
        const bool mailComposeAutosave = mHashConfig.value(mailComposeAutosaveStr).toBool();
        if (mailComposeAutosave) {
            const QString mailComposeAutosaveintervalStr = u"mail.compose.autosaveinterval"_s;
            if (mHashConfig.contains(mailComposeAutosaveintervalStr)) {
                bool found = false;
                const int mailComposeAutosaveinterval = mHashConfig.value(mailComposeAutosaveintervalStr).toInt(&found);
                if (found) {
                    addKmailConfig(u"Composer"_s, u"autosave"_s, mailComposeAutosaveinterval);
                } else {
                    addKmailConfig(u"Composer"_s, u"autosave"_s, 5);
                }
            } else {
                // Default value
                addKmailConfig(u"Composer"_s, u"autosave"_s, 5);
            }
        } else {
            // Don't autosave
            addKmailConfig(u"Composer"_s, u"autosave"_s, 0);
        }
    }

    const QString mailSpellCheckInlineStr = u"mail.spellcheck.inline"_s;
    if (mHashConfig.contains(mailSpellCheckInlineStr)) {
        const bool mailSpellCheckInline = mHashConfig.value(mailSpellCheckInlineStr).toBool();
        addKmailConfig(u"Spelling"_s, u"backgroundCheckerEnabled"_s, mailSpellCheckInline);
    } else {
        addKmailConfig(u"Spelling"_s, u"backgroundCheckerEnabled"_s, false);
    }

    const QString mailAlertShowSubjectStr = u"mail.biff.alert.show_subject"_s;
    if (mHashConfig.contains(mailAlertShowSubjectStr)) {
        const bool mailAlertShowSubject = mHashConfig.value(mailAlertShowSubjectStr).toBool();
        addNewMailNotifier(u"General"_s, u"showSubject"_s, mailAlertShowSubject);
    } else {
        // Default value in thunderbird
        addNewMailNotifier(u"General"_s, u"showSubject"_s, true);
    }

    const QString mailAlertShowPreviewStr = u"mail.biff.alert.show_preview"_s;
    // TODO add show preview
    if (mHashConfig.contains(mailAlertShowPreviewStr)) {
        const bool mailAlertShowPreview = mHashConfig.value(mailAlertShowPreviewStr).toBool();
        // addNewMailNotifier(u"General"_s,u"showSubject"_s, mailAlertShowPreview);
    } else {
        // Default value in thunderbird
        // addNewMailNotifier(u"General"_s,u"showSubject"_s, true);
    }

    const QString mailAlertShowSenderStr = u"mail.biff.alert.show_sender"_s;
    if (mHashConfig.contains(mailAlertShowSenderStr)) {
        const bool mailAlertShowSender = mHashConfig.value(mailAlertShowSenderStr).toBool();
        addNewMailNotifier(u"General"_s, u"showFrom"_s, mailAlertShowSender);
    } else {
        // Default value in thunderbird
        addNewMailNotifier(u"General"_s, u"showFrom"_s, true);
    }

    const QString mailSpellCheckBeforeSendStr = u"mail.SpellCheckBeforeSend"_s;
    if (mHashConfig.contains(mailSpellCheckBeforeSendStr)) {
        const bool mailSpellCheckBeforeSend = mHashConfig.value(mailSpellCheckBeforeSendStr).toBool();
        addKmailConfig(u"Composer"_s, u"check-spelling-before-send"_s, mailSpellCheckBeforeSend);
    } else {
        addKmailConfig(u"Composer"_s, u"check-spelling-before-send"_s, false);
    }

    const QString mailSpellCheckLanguageStr = u"spellchecker.dictionary"_s;
    if (mHashConfig.contains(mailSpellCheckLanguageStr)) {
        const QString mailSpellCheckLanguage = mHashConfig.value(mailSpellCheckLanguageStr).toString();
        addKmailConfig(u"Spelling"_s, u"defaultLanguage"_s, mailSpellCheckLanguage);
        // TODO create map to convert thunderbird name to aspell name
    }

    const QString mailPhishingDetectionStr = u"mail.phishing.detection.enabled"_s;
    if (mHashConfig.contains(mailPhishingDetectionStr)) {
        const bool mailPhishingDetectionEnabled = mHashConfig.value(mailPhishingDetectionStr).toBool();
        addKmailConfig(u"Reader"_s, u"ScamDetectionEnabled"_s, mailPhishingDetectionEnabled);
    } else { // Default
        addKmailConfig(u"Reader"_s, u"ScamDetectionEnabled"_s, true);
    }

    const QString mailDisplayGlyphStr = u"mail.display_glyph"_s;
    if (mHashConfig.contains(mailDisplayGlyphStr)) {
        const bool mailDisplayGlyphEnabled = mHashConfig.value(mailDisplayGlyphStr).toBool();
        addKmailConfig(u"Reader"_s, u"ShowEmoticons"_s, mailDisplayGlyphEnabled);
    } else { // Default
        addKmailConfig(u"Reader"_s, u"ShowEmoticons"_s, true);
    }
}

void ThunderbirdSettings::importSieveSettings(QMap<QString, QVariant> &settings, const QString &userName, const QString &imapServerName)
{
    QString userNameSieveConverted = userName;
    userNameSieveConverted.replace(u'@', u"%40"_s);

    const QString sieveKeyServerUserName = u"extensions.sieve.account."_s + userNameSieveConverted + u'@' + imapServerName;
    // user_pref("extensions.sieve.account.<username>@<server>.enabled", true);
    if (mHashConfig.value(sieveKeyServerUserName + u".enabled"_s, false).toBool()) {
        settings.insert(u"SieveSupport"_s, true);
        settings.insert(u"SievePort"_s, mHashConfig.value(sieveKeyServerUserName + u".port"_s, 4190).toInt());
        // TODO add more
    }
#if 0
    <group name="siever">
      <entry name="SieveSupport" type="Bool">
        <label>Define if server supports sieve</label>
        <default>false</default>
      </entry>
      <entry name="SieveReuseConfig" type="Bool">
        <label>Define if we reuse host and login configuration</label>
        <default>true</default>
      </entry>
      <entry name="SievePort" type="Int">
        <label>Define sieve port</label>
        <default>4190</default>
      </entry>
      <entry name="SieveAlternateUrl" type="String">
        <label>Define alternate URL</label>
      </entry>
      <entry name="AlternateAuthentication" type="Int">
        <label>Defines the authentication type to use for alternate server</label>
        <default>1</default>
      </entry>
      <entry name="SieveVacationFilename" type="String">
        <label>Define default sieve vacation filename</label>
        <default>kmail-vacation.siv</default>
      </entry>
      <entry name="SieveCustomUsername" type="String">
        <label>Define username used from custom server sieve url</label>
        <default></default>
      </entry>
      <entry name="SieveCustomAuthentification" type="String">
        <label>Defines the type of identification used by custom sieve server</label>
        <default>ImapUserPassword</default>
      </entry>
    </group>

#endif
#if 0
    if (mHashConfig.value(sieveKeyServerUserName + u".enabled"_s, false).toBool()) {
        //TODO
        //user_pref("extensions.sieve.account.<username>@<server>.TLS", true);
        //user_pref("extensions.sieve.account.<username>@<server>.TLS.forced", true);
        //user_pref("extensions.sieve.account.<username>@<server>.activeAuthorization", 1);
        //user_pref("extensions.sieve.account.<username>@<server>.activeHost", 1);
        //user_pref("extensions.sieve.account.<username>@<server>.activeLogin", 1);
        SieveEditorUtil::SieveServerConfig config;
        //0 4190
        //1 2000
        //2 custom
        //Default == 4190
        //user_pref("extensions.sieve.account.<username>@<server>.port", 1255);
        config.sieveSettings.port = mHashConfig.value(sieveKeyServerUserName + u".port"_s, 4190).toInt();
        //not necessary to import this one : user_pref("extensions.sieve.account.<username>@<server>.port.type", 1);

        //user_pref("extensions.sieve.account.<username>@<server>.hostname", "sdfsfsqsdf");
        const QString sieveHostName = mHashConfig.value(sieveKeyServerUserName + u".hostname"_s).toString();
        if (sieveHostName.isEmpty()) {
            config.sieveSettings.serverName = imapServerName;
        } else {
            config.sieveSettings.serverName = sieveHostName;
        }

        const QString sieveUserName = mHashConfig.value(sieveKeyServerUserName + u".login.username"_s).toString();
        //user_pref("extensions.sieve.account.<username>@<server>.login.username", "newuser");
        if (sieveUserName.isEmpty()) {
            config.sieveSettings.userName = userName;
        } else {
            config.sieveSettings.userName = sieveUserName;
        }

        //not necessary to import this one : user_pref("extensions.sieve.account.<username>@<server>.proxy.type", 1);


        ////FIXME qCDebug(SIEVEEDITOR_LOG) << "imap account " << accountName;
        const QString name = mHashConfig.value(accountName + u".name"_s).toString();
        bool found;
        const int sievePort = mHashConfig.value(accountName + u".port"_s).toInt(&found);
        if (found) {
            config.sieveImapAccountSettings.setPort(sievePort);
        }
        encryption(config, accountName);
        addAuth(config, accountName);
        config.sieveImapAccountSettings.setUserName(userName);
        config.sieveImapAccountSettings.setServerName(imapServerName);

        if (config.isValid()) {
            atLeastAnAccountFound = true;
            Q_EMIT importSetting(name, config);
        }
    }

#endif
}

void ThunderbirdSettings::addAuth(QMap<QString, QVariant> &settings, const QString &argument, const QString &accountName)
{
    bool found = false;
    if (mHashConfig.contains(accountName + u".authMethod"_s)) {
        const int authMethod = mHashConfig.value(accountName + u".authMethod"_s).toInt(&found);
        if (found) {
            switch (authMethod) {
            case 0:
                settings.insert(argument, MailTransport::Transport::EnumAuthenticationType::PLAIN); //????
                break;
            case 4: // Encrypted password ???
                settings.insert(argument, MailTransport::Transport::EnumAuthenticationType::LOGIN); //????
                qCDebug(THUNDERBIRDPLUGIN_LOG) << " authmethod == encrypt password";
                break;
            case 5: // GSSAPI
                settings.insert(argument, MailTransport::Transport::EnumAuthenticationType::GSSAPI);
                break;
            case 6: // NTLM
                settings.insert(argument, MailTransport::Transport::EnumAuthenticationType::NTLM);
                break;
            case 7: // TLS
                qCDebug(THUNDERBIRDPLUGIN_LOG) << " authmethod method == TLS"; //????
                break;
            case 10: // OAuth2 verify it.
                settings.insert(argument, MailTransport::Transport::EnumAuthenticationType::XOAUTH2);
                qCDebug(THUNDERBIRDPLUGIN_LOG) << " authmethod method == OAuth2"; //????
                break;
            default:
                qCDebug(THUNDERBIRDPLUGIN_LOG) << " ThunderbirdSettings::addAuth unknown :" << authMethod;
                break;
            }
        }
    }
}

void ThunderbirdSettings::readAccount()
{
    for (const QString &account : std::as_const(mAccountList)) {
        const QString serverName = mHashConfig.value(u"mail.account.%1"_s.arg(account) + u".server"_s).toString();
        const QString accountName = u"mail.server.%1"_s.arg(serverName);
        const QString host = mHashConfig.value(accountName + u".hostname"_s).toString();
        const QString userName = mHashConfig.value(accountName + u".userName"_s).toString();
        const QString name = mHashConfig.value(accountName + u".name"_s).toString();

        const QString type = mHashConfig.value(accountName + u".type"_s).toString();
        // TODO use it ?
        // const QString directory = mHashConfig.value(accountName + u".directory"_s).toString();

        const QString loginAtStartupStr = accountName + u".login_at_startup"_s;
        bool loginAtStartup = true; // Default for thunderbird;
        if (mHashConfig.contains(loginAtStartupStr)) {
            loginAtStartup = mHashConfig.value(loginAtStartupStr).toBool();
        }
        bool found = false;
        if (type == QLatin1StringView("imap")) {
            QMap<QString, QVariant> settings;
            settings.insert(u"ImapServer"_s, host);
            settings.insert(u"UserName"_s, userName);
            const int port = mHashConfig.value(accountName + u".port"_s).toInt(&found);
            if (found) {
                settings.insert(u"ImapPort"_s, port);
            }
            addAuth(settings, u"Authentication"_s, accountName);
            const QString offline = accountName + u".offline_download"_s;
            if (mHashConfig.contains(offline)) {
                const bool offlineStatus = mHashConfig.value(offline).toBool();
                if (offlineStatus) {
                    settings.insert(u"DisconnectedModeEnabled"_s, offlineStatus);
                }
            } else {
                // default value == true
                settings.insert(u"DisconnectedModeEnabled"_s, true);
            }

            found = false;
            const int socketType = mHashConfig.value(accountName + u".socketType"_s).toInt(&found);
            if (found) {
                switch (socketType) {
                case 0:
                    // None
                    settings.insert(u"Safety"_s, u"None"_s);
                    break;
                case 2:
                    // STARTTLS
                    settings.insert(u"Safety"_s, u"STARTTLS"_s);
                    break;
                case 3:
                    // SSL/TLS
                    settings.insert(u"Safety"_s, u"SSL"_s);
                    break;
                default:
                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " socketType " << socketType;
                    break;
                }
            }
            const QString checkNewMailStr = accountName + u".check_new_mail"_s;
            if (mHashConfig.contains(checkNewMailStr)) {
                const bool checkNewMail = mHashConfig.value(checkNewMailStr).toBool();
                settings.insert(u"IntervalCheckEnabled"_s, checkNewMail);
            }

            const QString checkTimeStr = accountName + u".check_time"_s;
            if (mHashConfig.contains(checkTimeStr)) {
                found = false;
                const int checkTime = mHashConfig.value(checkTimeStr).toInt(&found);
                if (found) {
                    settings.insert(u"IntervalCheckTime"_s, checkTime);
                }
            } else {
                // Default value from thunderbird
                settings.insert(u"IntervalCheckTime"_s, 10);
            }
            const QString trashFolderStr = accountName + u".trash_folder_name"_s;
            if (mHashConfig.contains(trashFolderStr)) {
                settings.insert(u"TrashCollection"_s, MailCommon::Util::convertFolderPathToCollectionId(mHashConfig.value(trashFolderStr).toString()));
            }
            importSieveSettings(settings, userName, serverName);

            const QString agentIdentifyName = LibImportWizard::AbstractBase::createResource(u"akonadi_imap_resource"_s, name, settings);
            addCheckMailOnStartup(agentIdentifyName, loginAtStartup);
            // Not find a method to disable it in thunderbird
            addToManualCheck(agentIdentifyName, true);
        } else if (type == QLatin1StringView("pop3")) {
            QMap<QString, QVariant> settings;
            settings.insert(u"Host"_s, host);
            settings.insert(u"Login"_s, userName);

            const bool leaveOnServer = mHashConfig.value(accountName + u".leave_on_server"_s).toBool();
            if (leaveOnServer) {
                settings.insert(u"LeaveOnServer"_s, leaveOnServer);
            }

            found = false;
            const int numberDayToLeave = mHashConfig.value(accountName + u".num_days_to_leave_on_server"_s).toInt(&found);
            if (found) {
                settings.insert(u"LeaveOnServerDays"_s, numberDayToLeave);
            }

            found = false;
            const int port = mHashConfig.value(accountName + u".port"_s).toInt(&found);
            if (found) {
                settings.insert(u"Port"_s, port);
            }

            found = false;
            const int socketType = mHashConfig.value(accountName + u".socketType"_s).toInt(&found);
            if (found) {
                switch (socketType) {
                case 0:
                    // None
                    // nothing
                    break;
                case 2:
                    // STARTTLS
                    settings.insert(u"UseTLS"_s, true);
                    break;
                case 3:
                    // SSL/TLS
                    settings.insert(u"UseSSL"_s, true);
                    break;
                default:
                    qCDebug(THUNDERBIRDPLUGIN_LOG) << " socketType " << socketType;
                    break;
                }
            }
            addAuth(settings, u"AuthenticationMethod"_s, accountName);
            const QString checkNewMailStr = accountName + u".check_new_mail"_s;
            if (mHashConfig.contains(checkNewMailStr)) {
                const bool checkNewMail = mHashConfig.value(checkNewMailStr).toBool();
                settings.insert(u"IntervalCheckEnabled"_s, checkNewMail);
            }
            const QString checkTimeStr = accountName + u".check_time"_s;
            if (mHashConfig.contains(checkTimeStr)) {
                found = false;
                const int checkTime = mHashConfig.value(checkTimeStr).toInt(&found);
                if (found) {
                    settings.insert(u"IntervalCheckInterval"_s, checkTime);
                }
            } else {
                // Default value from thunderbird
                settings.insert(u"IntervalCheckInterval"_s, 10);
            }

            const QString agentIdentifyName = LibImportWizard::AbstractBase::createResource(u"akonadi_pop3_resource"_s, name, settings);
            addCheckMailOnStartup(agentIdentifyName, loginAtStartup);
            // Not find a method to disable it in thunderbird
            addToManualCheck(agentIdentifyName, true);
        } else if (type == QLatin1StringView("none")) {
            // FIXME look at if we can implement it
            qCDebug(THUNDERBIRDPLUGIN_LOG) << " account type none!";
        } else if (type == QLatin1StringView("rss") || type == QLatin1StringView("nntp") || type == QLatin1StringView("movemail")) {
            qCDebug(THUNDERBIRDPLUGIN_LOG) << u"%1 rss resource needs to be implemented"_s.arg(type);
            continue;
        } else {
            qCDebug(THUNDERBIRDPLUGIN_LOG) << " type unknown : " << type;
            continue;
        }

        const QString identityConfig = u"mail.account.%1"_s.arg(account) + u".identities"_s;
        if (mHashConfig.contains(identityConfig)) {
            const QStringList idList = mHashConfig.value(identityConfig).toString().split(u',');
            for (const QString &id : idList) {
                readIdentity(id);
            }
        }
    }
}

void ThunderbirdSettings::readTransport()
{
    const QString mailSmtpServer = mHashConfig.value(u"mail.smtpservers"_s).toString();
    if (mailSmtpServer.isEmpty()) {
        return;
    }
    const QStringList smtpList = mailSmtpServer.split(u',');
    QString defaultSmtp = mHashConfig.value(u"mail.smtp.defaultserver"_s).toString();
    if (smtpList.count() == 1 && defaultSmtp.isEmpty()) {
        // Be sure to define default smtp
        defaultSmtp = smtpList.at(0);
    }

    for (const QString &smtp : smtpList) {
        const QString smtpName = u"mail.smtpserver.%1"_s.arg(smtp);
        MailTransport::Transport *mt = createTransport();
        mt->setIdentifier(u"SMTP"_s);
        const QString name = mHashConfig.value(smtpName + u".description"_s).toString();
        mt->setName(name);
        const QString hostName = mHashConfig.value(smtpName + u".hostname"_s).toString();
        mt->setHost(hostName);

        const int port = mHashConfig.value(smtpName + u".port"_s).toInt();
        if (port > 0) {
            mt->setPort(port);
        }

        const int authMethod = mHashConfig.value(smtpName + u".authMethod"_s).toInt();
        switch (authMethod) {
        case 0:
            break;
        case 1: // No authentication
            mt->setAuthenticationType(MailTransport::Transport::EnumAuthenticationType::PLAIN); //????
            break;
        case 3: // Unencrypted password
            mt->setAuthenticationType(MailTransport::Transport::EnumAuthenticationType::CLEAR); //???
            break;
        case 4: // encrypted password
            mt->setAuthenticationType(MailTransport::Transport::EnumAuthenticationType::LOGIN); //???
            break;
        case 5: // GSSAPI
            mt->setAuthenticationType(MailTransport::Transport::EnumAuthenticationType::GSSAPI);
            break;
        case 6: // NTLM
            mt->setAuthenticationType(MailTransport::Transport::EnumAuthenticationType::NTLM);
            break;
        default:
            qCDebug(THUNDERBIRDPLUGIN_LOG) << " authMethod unknown :" << authMethod;
            break;
        }

        const int trySsl = mHashConfig.value(smtpName + u".try_ssl"_s).toInt();
        switch (trySsl) {
        case 0:
            mt->setEncryption(MailTransport::Transport::EnumEncryption::None);
            break;
        case 2:
            mt->setEncryption(MailTransport::Transport::EnumEncryption::TLS);
            break;
        case 3:
            mt->setEncryption(MailTransport::Transport::EnumEncryption::SSL);
            break;
        default:
            qCDebug(THUNDERBIRDPLUGIN_LOG) << " trySsl unknown :" << trySsl;
            break;
        }

        const QString userName = mHashConfig.value(smtpName + u".username"_s).toString();
        if (!userName.isEmpty()) {
            mt->setUserName(userName);
            if (authMethod > 1) {
                mt->setRequiresAuthentication(true);
            }
        }

        storeTransport(mt, (smtp == defaultSmtp));
        mHashSmtp.insert(smtp, QString::number(mt->id()));
    }
}

static QString convertThunderbirdPath(const QString &path)
{
    QString newPath;
    QUrl url(path);
    newPath = url.path();
    newPath.remove(0, 1);
    return MailCommon::Util::convertFolderPathToCollectionStr(newPath);
}

void ThunderbirdSettings::readIdentity(const QString &account)
{
    const QString identity = u"mail.identity.%1"_s.arg(account);
    QString fullName = mHashConfig.value(identity + u".fullName"_s).toString();
    KIdentityManagementCore::Identity *newIdentity = createIdentity(fullName);

    const QString smtpServer = mHashConfig.value(identity + u".smtpServer"_s).toString();
    if (!smtpServer.isEmpty() && mHashSmtp.contains(smtpServer)) {
        newIdentity->setTransport(mHashSmtp.value(smtpServer));
    }

    const QString userEmail = mHashConfig.value(identity + u".useremail"_s).toString();
    newIdentity->setPrimaryEmailAddress(userEmail);

    newIdentity->setFullName(fullName);
    newIdentity->setIdentityName(fullName);

    const QString organization = mHashConfig.value(identity + u".organization"_s).toString();
    newIdentity->setOrganization(organization);

    bool doBcc = mHashConfig.value(identity + u".doBcc"_s).toBool();
    if (doBcc) {
        const QString bcc = mHashConfig.value(identity + u".doBccList"_s).toString();
        newIdentity->setBcc(bcc);
    }

    bool doCc = mHashConfig.value(identity + u".doCc"_s).toBool();
    if (doCc) {
        const QString cc = mHashConfig.value(identity + u".doCcList"_s).toString();
        newIdentity->setCc(cc);
    }

    const QString replyTo = mHashConfig.value(identity + u".reply_to"_s).toString();
    newIdentity->setReplyToAddr(replyTo);

    KIdentityManagementCore::Signature signature;
    const bool signatureHtml = mHashConfig.value(identity + u".htmlSigFormat"_s).toBool();
    if (signatureHtml) {
        signature.setInlinedHtml(true);
    }

    const bool attachSignature = mHashConfig.value(identity + u".attach_signature"_s).toBool();
    if (attachSignature) {
        const QString fileSignature = mHashConfig.value(identity + u".sig_file"_s).toString();
        signature.setType(KIdentityManagementCore::Signature::FromFile);
        signature.setPath(fileSignature, false);
    } else {
        const QString textSignature = mHashConfig.value(identity + u".htmlSigText"_s).toString();
        signature.setType(KIdentityManagementCore::Signature::Inlined);
        signature.setText(textSignature);
    }

    if (mHashConfig.contains(identity + u".drafts_folder_picker_mode"_s)) {
        const int useSpecificDraftFolder = mHashConfig.value(identity + u".drafts_folder_picker_mode"_s).toInt();
        if (useSpecificDraftFolder == 1) {
            const QString draftFolder = convertThunderbirdPath(mHashConfig.value(identity + u".draft_folder"_s).toString());
            newIdentity->setDrafts(draftFolder);
        }
    }

    if (mHashConfig.contains(identity + u".fcc"_s)) {
        const bool fccEnabled = mHashConfig.value(identity + u".fcc"_s).toBool();
        newIdentity->setDisabledFcc(!fccEnabled);
    }

    // fcc_reply_follows_parent not implemented in kmail
    // fcc_folder_picker_mode is just a flag for thunderbird. Not necessary during import.
    // if ( mHashConfig.contains( identity + QStringLiteral( ".fcc_folder_picker_mode" ) ) )
    {
        if (mHashConfig.contains(identity + u".fcc_folder"_s)) {
            const QString fccFolder = convertThunderbirdPath(mHashConfig.value(identity + u".fcc_folder"_s).toString());
            newIdentity->setFcc(fccFolder);
        }
    }

    // if ( mHashConfig.contains( identity + QStringLiteral( ".tmpl_folder_picker_mode" ) ) )
    {
        if (mHashConfig.contains(identity + u".stationery_folder"_s)) {
            const QString templateFolder = convertThunderbirdPath(mHashConfig.value(identity + u".stationery_folder"_s).toString());
            newIdentity->setTemplates(templateFolder);
        }
    }

    const QString attachVcardStr(identity + u".attach_vcard"_s);
    if (mHashConfig.contains(attachVcardStr)) {
        const bool attachVcard = mHashConfig.value(attachVcardStr).toBool();
        newIdentity->setAttachVcard(attachVcard);
    }
    const QString attachVcardContentStr(identity + u".escapedVCard"_s);
    if (mHashConfig.contains(attachVcardContentStr)) {
        const QString str = mHashConfig.value(attachVcardContentStr).toString();
        QByteArray vcard = QByteArray::fromPercentEncoding(str.toLocal8Bit());
        KContacts::VCardConverter converter;
        KContacts::Addressee addr = converter.parseVCard(vcard);

        const QString filename = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation) + u'/' + newIdentity->identityName() + u".vcf"_s;
        QFileInfo fileInfo(filename);
        QDir().mkpath(fileInfo.absolutePath());
        QFile file(filename);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
            const QByteArray data = converter.exportVCard(addr, KContacts::VCardConverter::v3_0);
            file.write(data);
            file.flush();
            file.close();
            newIdentity->setVCardFile(filename);
        }
    }
    const QString signMailStr(identity + u".sign_mail"_s);
    if (mHashConfig.contains(signMailStr)) {
        const bool signMail = mHashConfig.value(signMailStr).toBool();
        newIdentity->setPgpAutoSign(signMail);
    }

    // const QString composeHtmlStr(identity + u".compose_html"_s);
    // TODO: implement it in kmail

    newIdentity->setSignature(signature);

    storeIdentity(newIdentity);
}

void ThunderbirdSettings::insertIntoMap(const QString &line)
{
    QString newLine = line;
    newLine.remove(u"user_pref(\""_s);
    newLine.remove(u");"_s);
    const int pos = newLine.indexOf(u',');
    QString key = newLine.left(pos);
    key.remove(key.length() - 1, 1);
    QString valueStr = newLine.right(newLine.length() - pos - 2);
    if (valueStr.at(0) == u'"') {
        valueStr.remove(0, 1);
        const int pos(valueStr.length() - 1);
        if (valueStr.at(pos) == u'"') {
            valueStr.remove(pos, 1);
        }
        // Store as String
        mHashConfig.insert(key, valueStr);
    } else {
        if (valueStr == QLatin1StringView("true")) {
            mHashConfig.insert(key, true);
        } else if (valueStr == QLatin1StringView("false")) {
            mHashConfig.insert(key, false);
        } else {
            // Store as integer
            const int value = valueStr.toInt();
            mHashConfig.insert(key, value);
        }
    }
    if (key.contains(QLatin1StringView("ldap_")) && key.endsWith(QLatin1StringView(".description"))) {
        QString ldapAccountName = key;
        mLdapAccountList.append(ldapAccountName.remove(u".description"_s));
    }
    if (key.contains(QLatin1StringView("mailnews.tags.")) && (key.endsWith(QLatin1StringView(".color")) || key.endsWith(QLatin1StringView(".tag")))) {
        QString name = key;
        name.remove(u"mailnews.tags."_s);
        name.remove(u".color"_s);
        name.remove(u".tag"_s);
        tagStruct tag;
        if (mHashTag.contains(name)) {
            tag = mHashTag.value(name);
            mHashTag.remove(name);
        }
        if (key.endsWith(QLatin1StringView(".color"))) {
            tag.color = QColor(mHashConfig.value(key).toString());
        } else {
            tag.name = mHashConfig.value(key).toString();
        }
        mHashTag.insert(name, tag);
        qCDebug(THUNDERBIRDPLUGIN_LOG) << " tag :" << name << " tag.name" << tag.name << " color :" << tag.color;
    }
}

void ThunderbirdSettings::addNewMailNotifier(const QString &group, const QString &key, bool value)
{
    KConfig config(u"akonadi_newmailnotifier_agentrc"_s);
    KConfigGroup grp = config.group(group);
    grp.writeEntry(key, value);
}
