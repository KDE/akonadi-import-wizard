/*
   Copyright (C) 2017 Montel Laurent <montel@kde.org>

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

#include "gearysettings.h"
#include "importwizardutil.h"

#include <mailtransport/transportmanager.h>

#include <KIdentityManagement/kidentitymanagement/identity.h>
#include <KIdentityManagement/kidentitymanagement/signature.h>

#include "importwizard_debug.h"

#include <QSettings>

GearySettings::GearySettings(const QString &filename, ImportWizard *parent)
    : AbstractSettings(parent)
{
    settings = new QSettings(filename, QSettings::IniFormat, this);
}

GearySettings::~GearySettings()
{
    delete settings;
}

void GearySettings::importSettings()
{
    readImapAccount();
    readTransport();
    readIdentity();
    readGlobalSettings();
}

void GearySettings::readImapAccount()
{
    QMap<QString, QVariant> newSettings;

    QString name;

    if (settings->contains(QStringLiteral("imap_host"))) {
        name = settings->value(QStringLiteral("imap_host")).toString();
        newSettings.insert(QStringLiteral("ImapServer"), name);
    }

    if (settings->contains(QStringLiteral("imap_port"))) {
        int port = settings->value(QStringLiteral("imap_port")).toInt();
        newSettings.insert(QStringLiteral("ImapPort"), port);
    }

    if (settings->contains(QStringLiteral("imap_starttls"))) {
        const bool useTLS = settings->value(QStringLiteral("imap_starttls")).toBool();
        if (useTLS) {
            newSettings.insert(QStringLiteral("Safety"), QStringLiteral("STARTTLS"));
        }
    }

    const QString userName = settings->value(QStringLiteral("imap_username")).toString();
    if (!userName.isEmpty()) {
        newSettings.insert(QStringLiteral("Username"), userName);
    }

    if (!name.isEmpty()) {
        const QString agentIdentifyName = AbstractBase::createResource(QStringLiteral("akonadi_imap_resource"), name, newSettings);
        //Check by default
        addCheckMailOnStartup(agentIdentifyName, true);
    }
}

void GearySettings::readTransport()
{
}

void GearySettings::readIdentity()
{
}

void GearySettings::readGlobalSettings()
{
    //TODO
}
