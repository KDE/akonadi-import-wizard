/*
   SPDX-FileCopyrightText: 2012-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "abstractsettings.h"

#include <QString>

class KConfigGroup;
class QFile;

class SylpheedSettings : public AbstractSettings
{
public:
    SylpheedSettings();
    ~SylpheedSettings() override;
    virtual void importSettings(const QString &filename, const QString &path);

protected:
    void readCustomHeader(QFile *customHeaderFile);
    virtual void readGlobalSettings(const KConfigGroup &group);
    void readAccount(const KConfigGroup &accountConfig, bool checkMailOnStartup, int intervalCheckMail);
    void readIdentity(const KConfigGroup &accountConfig);
    QString readTransport(const KConfigGroup &accountConfig);
    void readPop3Account(const KConfigGroup &accountConfig, bool checkMailOnStartup, int intervalCheckMail);
    void readImapAccount(const KConfigGroup &accountConfig, bool checkMailOnStartup, int intervalCheckMail);
    void readSignature(const KConfigGroup &accountConfig, KIdentityManagementCore::Identity *identity);

    virtual void readSettingsColor(const KConfigGroup &group);
    virtual void readTemplateFormat(const KConfigGroup &group);

    virtual void readTagColor(const KConfigGroup &group);

    virtual void readDateFormat(const KConfigGroup &group);
    QString convertToKmailTemplate(const QString &templateStr);
};
