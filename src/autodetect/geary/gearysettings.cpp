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

    name = settings->value(QStringLiteral("imap_host")).toString();
    if (!name.isEmpty()) {
        newSettings.insert(QStringLiteral("ImapServer"), name);
    }

    const int port = settings->value(QStringLiteral("imap_port"), -1).toInt();
    if (port > -1) {
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
#if 0
    smtp_host=smtp.bla.bla
    smtp_port=465
    smtp_ssl=true
    smtp_starttls=false
    smtp_use_imap_credentials=false
    smtp_noauth=true
#endif
}

void GearySettings::readIdentity()
{
#if 0
    real_name=blo
    primary_email=bli@kde.org
    nickname=bli@kde.org
#endif
}

void GearySettings::readGlobalSettings()
{
    //TODO
}
