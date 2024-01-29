/*
   SPDX-FileCopyrightText: 2012-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "clawsmailsettings.h"
#include "importwizardutil.h"

#include <MailCommon/MailUtil>
#include <MailTransport/TransportManager>

#include <KIdentityManagementCore/Identity>

#include "clawsmailplugin_debug.h"
#include <KConfig>
#include <KConfigGroup>
#include <QFileInfo>

#include <QRegularExpression>

ClawsMailSettings::ClawsMailSettings() = default;

ClawsMailSettings::~ClawsMailSettings() = default;

void ClawsMailSettings::importSettings(const QString &filename, const QString &path)
{
    bool checkMailOnStartup = true;
    int intervalCheckMail = -1;
    const QString clawsmailrc = path + QLatin1StringView("/clawsrc");
    if (QFileInfo::exists(clawsmailrc)) {
        KConfig configCommon(clawsmailrc);
        if (configCommon.hasGroup(QLatin1StringView("Common"))) {
            KConfigGroup common = configCommon.group(QStringLiteral("Common"));
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
    const QString customheaderrc = path + QLatin1StringView("/customheaderrc");
    QFile customHeaderFile(customheaderrc);
    if (customHeaderFile.exists()) {
        if (!customHeaderFile.open(QIODevice::ReadOnly)) {
            qCDebug(CLAWSMAILPLUGIN_LOG) << " We can't open file" << customheaderrc;
        } else {
            readCustomHeader(&customHeaderFile);
        }
    }
}

void ClawsMailSettings::readSettingsColor(const KConfigGroup &group)
{
    const bool enableColor = group.readEntry("enable_color", false);
    if (enableColor) {
        const QString colorLevel1 = group.readEntry("quote_level1_color");
        if (!colorLevel1.isEmpty()) {
            const QColor col = QColor(colorLevel1);
            if (col.isValid()) {
                addKmailConfig(QStringLiteral("Reader"), QStringLiteral("QuotedText1"), writeColor(col));
            }
            //[Reader]  QuotedText1
        }
        const QString colorLevel2 = group.readEntry("quote_level2_color");
        if (!colorLevel2.isEmpty()) {
            const QColor col = QColor(colorLevel2);
            if (col.isValid()) {
                addKmailConfig(QStringLiteral("Reader"), QStringLiteral("QuotedText2"), writeColor(col));
            }
            //[Reader]  QuotedText2
        }
        const QString colorLevel3 = group.readEntry("quote_level3_color");
        if (!colorLevel3.isEmpty()) {
            const QColor col = QColor(colorLevel3);
            if (col.isValid()) {
                addKmailConfig(QStringLiteral("Reader"), QStringLiteral("QuotedText3"), writeColor(col));
            }
            //[Reader]  QuotedText3
        }
        const QString misspellColor = group.readEntry(QStringLiteral("misspelled_color"));
        if (!misspellColor.isEmpty()) {
            const QColor col = QColor(misspellColor);
            if (col.isValid()) {
                addKmailConfig(QStringLiteral("Reader"), QStringLiteral("MisspelledColor"), writeColor(col));
            }
        }
        const QString uriColor = group.readEntry(QStringLiteral("uri_color"));
        if (!uriColor.isEmpty()) {
            const QColor col(uriColor);
            if (col.isValid()) {
                addKmailConfig(QStringLiteral("Reader"), QStringLiteral("LinkColor"), writeColor(col));
            }
        }
        const QString newColor = group.readEntry(QStringLiteral("color_new"));
        if (!newColor.isEmpty()) {
            const QColor col(newColor);
            if (col.isValid()) {
                addKmailConfig(QStringLiteral("MessageListView::Colors"), QStringLiteral("UnreadMessageColor"), writeColor(col));
            }
        }
    }
}

QString ClawsMailSettings::writeColor(const QColor &col)
{
    QStringList list;
    list.insert(0, QString::number(col.red()));
    list.insert(1, QString::number(col.green()));
    list.insert(2, QString::number(col.blue()));
    if (col.alpha() != 255) {
        list.insert(3, QString::number(col.alpha()));
    }
    return list.join(QLatin1Char(','));
}

void ClawsMailSettings::readTemplateFormat(const KConfigGroup &group)
{
    SylpheedSettings::readTemplateFormat(group);
    const QString composerNewMessage = group.readEntry(QStringLiteral("compose_body_format"));
    if (!composerNewMessage.isEmpty()) {
        addKmailConfig(QStringLiteral("TemplateParser"), QStringLiteral("TemplateNewMessage"), convertToKmailTemplate(composerNewMessage));
    }
}

void ClawsMailSettings::readGlobalSettings(const KConfigGroup &group)
{
    SylpheedSettings::readGlobalSettings(group);
    if (group.readEntry(QStringLiteral("check_while_typing"), 0) == 1) {
        addKmailConfig(QStringLiteral("Spelling"), QStringLiteral("backgroundCheckerEnabled"), true);
    }
    const int markAsRead = group.readEntry(QStringLiteral("mark_as_read_delay"), -1);
    if (markAsRead != -1) {
        addKmailConfig(QStringLiteral("Behaviour"), QStringLiteral("DelayedMarkTime"), markAsRead);
        addKmailConfig(QStringLiteral("Behaviour"), QStringLiteral("DelayedMarkAsRead"), true);
    }

    const int warnLargeFileInserting = group.readEntry(QStringLiteral("warn_large_insert"), 0);
    if (warnLargeFileInserting == 0) {
        addKmailConfig(QStringLiteral("Composer"), QStringLiteral("MaximumAttachmentSize"), -1);
    } else {
        const int warnLargeFileSize = group.readEntry(QStringLiteral("warn_large_insert_size"), -1);
        if (warnLargeFileSize > 0) {
            addKmailConfig(QStringLiteral("Composer"), QStringLiteral("MaximumAttachmentSize"), warnLargeFileSize * 1024);
        }
    }
}

void ClawsMailSettings::readTagColor(const KConfigGroup &group)
{
    const QString customColorPattern(QStringLiteral("custom_color%1"));
    const QString customColorLabelPattern(QStringLiteral("custom_colorlabel%1"));
    QList<tagStruct> listTag;
    for (int i = 1; i <= 15; ++i) {
        if (group.hasKey(customColorPattern.arg(i)) && group.hasKey(customColorLabelPattern.arg(i))) {
            tagStruct tag;
            const QString colorStr = group.readEntry(customColorPattern.arg(i));
            const QString labelStr = group.readEntry(customColorLabelPattern.arg(i));
            if (!colorStr.isEmpty() && !labelStr.isEmpty()) {
                tag.color = QColor(colorStr).name();
                tag.name = labelStr;
                listTag << tag;
            }
        }
    }
    if (!listTag.isEmpty()) {
        ImportWizardUtil::addAkonadiTag(listTag);
    }
}
