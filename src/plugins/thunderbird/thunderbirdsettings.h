/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef THUNDERBIRDSETTINGS_H
#define THUNDERBIRDSETTINGS_H

#include "abstractsettings.h"
#include "importwizardutil.h"
#include <QHash>
#include <QStringList>

class ThunderbirdSettings : public AbstractSettings
{
public:
    explicit ThunderbirdSettings(const QString &filename);
    ~ThunderbirdSettings();
    void importSettings();
private:
    void readAccount();
    void readIdentity(const QString &account);
    void readTransport();
    void readGlobalSettings();
    void readLdapSettings();
    void readTagSettings();
    void readExtensionsSettings();
    int adaptAutoResizeResolution(int index, const QString &configStrList);

    void insertIntoMap(const QString &line);

    void addAuth(QMap<QString, QVariant> &settings, const QString &argument, const QString &accountName);
    void importSieveSettings(QMap<QString, QVariant> &settings, const QString &userName, const QString &imapServerName);

    void addNewMailNotifier(const QString &group, const QString &key, bool value);

    QHash<QString, QVariant> mHashConfig;
    QHash<QString, QString> mHashSmtp;
    QStringList mAccountList;
    QStringList mLdapAccountList;

    QHash<QString, tagStruct> mHashTag;
    QString mFileName;
};

#endif /* THUNDERBIRDSETTINGS_H */
