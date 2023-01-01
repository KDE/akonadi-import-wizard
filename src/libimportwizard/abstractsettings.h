/*
   SPDX-FileCopyrightText: 2012-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "abstractbase.h"
#include "libimportwizard_export.h"
#include <KSharedConfig>
#include <QMap>

namespace KIdentityManagement
{
class Identity;
class IdentityManager;
}

namespace MailTransport
{
class Transport;
}

class LIBIMPORTWIZARD_EXPORT AbstractSettings : public LibImportWizard::AbstractBase
{
    Q_OBJECT
public:
    AbstractSettings();
    ~AbstractSettings() override;

protected:
    void addImportInfo(const QString &log) override;
    void addImportError(const QString &log) override;

    void syncKmailConfig();

    QString uniqueIdentityName(const QString &name);

    QString createResource(const QString &resources, const QString &name, const QMap<QString, QVariant> &settings);

    KIdentityManagement::Identity *createIdentity(QString &name);

    MailTransport::Transport *createTransport();

    void storeTransport(MailTransport::Transport *mt, bool isDefault = false);

    void storeIdentity(KIdentityManagement::Identity *identity);

    void addKmailConfig(const QString &groupName, const QString &key, const QString &value);
    void addKmailConfig(const QString &groupName, const QString &key, bool value);
    void addKmailConfig(const QString &groupName, const QString &key, int value);

    void addComposerHeaderGroup(const QString &groupName, const QString &name, const QString &value);

    void addCheckMailOnStartup(const QString &agentIdentifyName, bool loginAtStartup);
    void addToManualCheck(const QString &agentIdentifyName, bool manualCheck);
    int readKmailSettings(const QString &groupName, const QString &key);

    KIdentityManagement::IdentityManager *mManager = nullptr;
    KSharedConfigPtr mKmailConfig;
};
