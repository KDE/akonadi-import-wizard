/*
   Copyright (C) 2012-2020 Laurent Montel <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef THUNDERBIRDSETTINGS_H
#define THUNDERBIRDSETTINGS_H

#include <ImportWizard/AbstractSettings>
#include <ImportWizard/ImportWizardUtil>
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
