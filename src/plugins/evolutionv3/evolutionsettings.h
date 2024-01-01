/*
   SPDX-FileCopyrightText: 2012-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "abstractsettings.h"
#include <KIdentityManagementCore/Signature>
#include <QString>

class QDomElement;

class EvolutionSettings : public AbstractSettings
{
public:
    EvolutionSettings();
    ~EvolutionSettings() override;
    void loadAccount(const QString &filename);
    void loadLdap(const QString &filename);

private:
    void readAccount(const QDomElement &account);
    void readLdap(const QString &account);
    void extractAccountInfo(const QString &info);
    void readSignatures(const QDomElement &account);
    void extractSignatureInfo(const QString &info);
    QString getSecurityMethod(const QStringList &listArgument, bool &found);
    QString getAuthMethod(const QString &path, bool &found);
    void addAuth(QMap<QString, QVariant> &settings, const QString &argument, const QString &userName);

    QMap<QString, KIdentityManagementCore::Signature> mMapSignature;
};
