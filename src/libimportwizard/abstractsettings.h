/*
   Copyright (C) 2012-2018 Montel Laurent <montel@kde.org>

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

#ifndef ABSTRACTSETTINGS_H
#define ABSTRACTSETTINGS_H

#include "abstractbase.h"
#include "libimportwizard_export.h"
#include <KSharedConfig>
#include <QMap>

namespace KIdentityManagement {
class Identity;
class IdentityManager;
}

namespace MailTransport {
class Transport;
}

class LIBIMPORTWIZARD_EXPORT AbstractSettings : public LibImportWizard::AbstractBase
{
    Q_OBJECT
public:
    AbstractSettings();
    ~AbstractSettings();

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

#endif // ABSTRACTSETTINGS_H
