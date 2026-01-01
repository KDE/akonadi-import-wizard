/*
   SPDX-FileCopyrightText: 2012-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "clawsmailsettings.h"
using namespace Qt::Literals::StringLiterals;

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
                addKmailConfig(u"Reader"_s, u"QuotedText1"_s, writeColor(col));
            }
            //[Reader]  QuotedText1
        }
        const QString colorLevel2 = group.readEntry("quote_level2_color");
        if (!colorLevel2.isEmpty()) {
            const QColor col = QColor(colorLevel2);
            if (col.isValid()) {
                addKmailConfig(u"Reader"_s, u"QuotedText2"_s, writeColor(col));
            }
            //[Reader]  QuotedText2
        }
        const QString colorLevel3 = group.readEntry("quote_level3_color");
        if (!colorLevel3.isEmpty()) {
            const QColor col = QColor(colorLevel3);
            if (col.isValid()) {
                addKmailConfig(u"Reader"_s, u"QuotedText3"_s, writeColor(col));
            }
            //[Reader]  QuotedText3
        }
        const QString misspellColor = group.readEntry(u"misspelled_color"_s);
        if (!misspellColor.isEmpty()) {
            const QColor col = QColor(misspellColor);
            if (col.isValid()) {
                addKmailConfig(u"Reader"_s, u"MisspelledColor"_s, writeColor(col));
            }
        }
        const QString uriColor = group.readEntry(u"uri_color"_s);
        if (!uriColor.isEmpty()) {
            const QColor col(uriColor);
            if (col.isValid()) {
                addKmailConfig(u"Reader"_s, u"LinkColor"_s, writeColor(col));
            }
        }
        const QString newColor = group.readEntry(u"color_new"_s);
        if (!newColor.isEmpty()) {
            const QColor col(newColor);
            if (col.isValid()) {
                addKmailConfig(u"MessageListView::Colors"_s, u"UnreadMessageColor"_s, writeColor(col));
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
    return list.join(u',');
}

void ClawsMailSettings::readTemplateFormat(const KConfigGroup &group)
{
    SylpheedSettings::readTemplateFormat(group);
    const QString composerNewMessage = group.readEntry(u"compose_body_format"_s);
    if (!composerNewMessage.isEmpty()) {
        addKmailConfig(u"TemplateParser"_s, u"TemplateNewMessage"_s, convertToKmailTemplate(composerNewMessage));
    }
}

void ClawsMailSettings::readGlobalSettings(const KConfigGroup &group)
{
    SylpheedSettings::readGlobalSettings(group);
    if (group.readEntry(u"check_while_typing"_s, 0) == 1) {
        addKmailConfig(u"Spelling"_s, u"backgroundCheckerEnabled"_s, true);
    }
    const int markAsRead = group.readEntry(u"mark_as_read_delay"_s, -1);
    if (markAsRead != -1) {
        addKmailConfig(u"Behaviour"_s, u"DelayedMarkTime"_s, markAsRead);
        addKmailConfig(u"Behaviour"_s, u"DelayedMarkAsRead"_s, true);
    }

    const int warnLargeFileInserting = group.readEntry(u"warn_large_insert"_s, 0);
    if (warnLargeFileInserting == 0) {
        addKmailConfig(u"Composer"_s, u"MaximumAttachmentSize"_s, -1);
    } else {
        const int warnLargeFileSize = group.readEntry(u"warn_large_insert_size"_s, -1);
        if (warnLargeFileSize > 0) {
            addKmailConfig(u"Composer"_s, u"MaximumAttachmentSize"_s, warnLargeFileSize * 1024);
        }
    }
}

void ClawsMailSettings::readTagColor(const KConfigGroup &group)
{
    const QString customColorPattern(u"custom_color%1"_s);
    const QString customColorLabelPattern(u"custom_colorlabel%1"_s);
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
