/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef EVOLUTIONSETTINGS_H
#define EVOLUTIONSETTINGS_H

#include "abstractsettings.h"
#include <KIdentityManagement/kidentitymanagement/signature.h>
#include <QString>

class QDomElement;

class EvolutionSettings : public AbstractSettings
{
public:
    EvolutionSettings();
    ~EvolutionSettings();
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

    QMap<QString, KIdentityManagement::Signature> mMapSignature;
};

#endif /* EVOLUTIONSETTINGS_H */
