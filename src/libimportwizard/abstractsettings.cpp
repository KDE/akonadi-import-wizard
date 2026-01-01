/*
   SPDX-FileCopyrightText: 2012-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "abstractsettings.h"
using namespace Qt::Literals::StringLiterals;

#include "abstractdisplayinfo.h"

#include <KIdentityManagementCore/Identity>
#include <KIdentityManagementCore/IdentityManager>
#include <MailTransport/TransportManager>

#include <KLocalizedString>

#include <Akonadi/AgentInstanceCreateJob>
#include <Akonadi/AgentManager>
#include <Akonadi/AgentType>

#include <QMetaMethod>

using namespace Akonadi;

AbstractSettings::AbstractSettings()
{
    mManager = KIdentityManagementCore::IdentityManager::self();
    mKmailConfig = KSharedConfig::openConfig(u"kmail2rc"_s);
}

AbstractSettings::~AbstractSettings()
{
    syncKmailConfig();
}

KIdentityManagementCore::Identity *AbstractSettings::createIdentity(QString &name)
{
    name = uniqueIdentityName(name);
    KIdentityManagementCore::Identity *identity = &mManager->newFromScratch(name);
    addImportInfo(i18n("Setting up identity..."));
    return identity;
}

void AbstractSettings::storeIdentity(KIdentityManagementCore::Identity *identity)
{
    mManager->setAsDefault(identity->uoid());
    mManager->commit();
    addImportInfo(i18n("Identity set up."));
}

QString AbstractSettings::uniqueIdentityName(const QString &name)
{
    QString newName(name);
    int i = 0;
    while (!mManager->isUnique(newName)) {
        newName = u"%1_%2"_s.arg(name).arg(i);
        ++i;
    }
    return newName;
}

MailTransport::Transport *AbstractSettings::createTransport()
{
    MailTransport::Transport *mt = MailTransport::TransportManager::self()->createTransport();
    addImportInfo(i18n("Setting up transport..."));
    return mt;
}

void AbstractSettings::storeTransport(MailTransport::Transport *mt, bool isDefault)
{
    mt->forceUniqueName();
    mt->save();
    MailTransport::TransportManager::self()->addTransport(mt);
    if (isDefault) {
        MailTransport::TransportManager::self()->setDefaultTransport(mt->id());
    }
    addImportInfo(i18n("Transport set up."));
}

void AbstractSettings::addImportInfo(const QString &log)
{
    Q_ASSERT_X(mAbstractDisplayInfo, __FUNCTION__, "mAbstractDisplayInfo is not setting");
    if (mAbstractDisplayInfo) {
        mAbstractDisplayInfo->settingsImportInfo(log);
    }
}

void AbstractSettings::addImportError(const QString &log)
{
    Q_ASSERT_X(mAbstractDisplayInfo, __FUNCTION__, "mAbstractDisplayInfo is not setting");
    if (mAbstractDisplayInfo) {
        mAbstractDisplayInfo->settingsImportError(log);
    }
}

void AbstractSettings::addCheckMailOnStartup(const QString &agentIdentifyName, bool loginAtStartup)
{
    if (agentIdentifyName.isEmpty()) {
        return;
    }
    const QString groupName = u"Resource %1"_s.arg(agentIdentifyName);
    addKmailConfig(groupName, u"CheckOnStartup"_s, loginAtStartup);
}

void AbstractSettings::addToManualCheck(const QString &agentIdentifyName, bool manualCheck)
{
    if (agentIdentifyName.isEmpty()) {
        return;
    }
    const QString groupName = u"Resource %1"_s.arg(agentIdentifyName);
    addKmailConfig(groupName, u"IncludeInManualChecks"_s, manualCheck);
}

void AbstractSettings::addComposerHeaderGroup(const QString &groupName, const QString &name, const QString &value)
{
    KConfigGroup group = mKmailConfig->group(groupName);
    group.writeEntry(u"name"_s, name);
    group.writeEntry(u"value"_s, value);
}

void AbstractSettings::addKmailConfig(const QString &groupName, const QString &key, const QString &value)
{
    KConfigGroup group = mKmailConfig->group(groupName);
    group.writeEntry(key, value);
}

void AbstractSettings::addKmailConfig(const QString &groupName, const QString &key, bool value)
{
    KConfigGroup group = mKmailConfig->group(groupName);
    group.writeEntry(key, value);
}

void AbstractSettings::addKmailConfig(const QString &groupName, const QString &key, int value)
{
    KConfigGroup group = mKmailConfig->group(groupName);
    group.writeEntry(key, value);
}

void AbstractSettings::syncKmailConfig()
{
    mKmailConfig->sync();
}

int AbstractSettings::readKmailSettings(const QString &groupName, const QString &key)
{
    KConfigGroup group = mKmailConfig->group(groupName);
    int value = group.readEntry(key, -1);
    return value;
}

#include "moc_abstractsettings.cpp"
