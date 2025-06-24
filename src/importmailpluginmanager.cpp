/*
   SPDX-FileCopyrightText: 2017-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "importmailpluginmanager.h"
using namespace Qt::Literals::StringLiterals;

#include "abstractimporter.h"
#include "importwizard_debug.h"
#include <KPluginFactory>
#include <KPluginMetaData>

namespace
{
QString pluginVersion()
{
    return u"1.0"_s;
}
}

ImportMailPluginManager::ImportMailPluginManager(QObject *parent)
    : QObject(parent)
{
    initializePluginList();
}

ImportMailPluginManager::~ImportMailPluginManager() = default;

ImportMailPluginManager *ImportMailPluginManager::self()
{
    static ImportMailPluginManager s_self;
    return &s_self;
}

bool ImportMailPluginManager::initializePluginList()
{
    const QList<KPluginMetaData> plugins = KPluginMetaData::findPlugins(u"pim6/importwizard"_s);
    QListIterator<KPluginMetaData> i(plugins);
    i.toBack();
    while (i.hasPrevious()) {
        ImportMailPluginManagerInfo info;
        const KPluginMetaData data = i.previous();

        // 1) get plugin data => name/description etc.
        info.pluginData = createPluginMetaData(data);
        info.metaDataFileName = data.fileName();
        info.data = data;
        if (pluginVersion() == data.version()) {
            info.plugin = nullptr;
            mPluginList.push_back(info);
        } else {
            qCWarning(IMPORTWIZARD_LOG) << "Plugin " << data.name() << " doesn't have correction plugin version. It will not be loaded.";
        }
    }
    QList<ImportMailPluginManagerInfo>::iterator end(mPluginList.end());
    for (QList<ImportMailPluginManagerInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
    return true;
}

void ImportMailPluginManager::loadPlugin(ImportMailPluginManagerInfo *item)
{
    if (auto plugin = KPluginFactory::instantiatePlugin<LibImportWizard::AbstractImporter>(item->data, this, QVariantList() << item->metaDataFileName).plugin) {
        item->plugin = plugin;
        mPluginDataList.append(item->pluginData);
    }
}

QList<LibImportWizard::AbstractImporter *> ImportMailPluginManager::pluginsList() const
{
    QList<LibImportWizard::AbstractImporter *> lst;
    QList<ImportMailPluginManagerInfo>::ConstIterator end(mPluginList.constEnd());
    for (QList<ImportMailPluginManagerInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if (auto plugin = (*it).plugin) {
            lst << plugin;
        }
    }
    return lst;
}

PluginUtilData ImportMailPluginManager::createPluginMetaData(const KPluginMetaData &metaData)
{
    PluginUtilData pluginData;
    pluginData.mName = metaData.name();
    pluginData.mIdentifier = metaData.pluginId();
    return pluginData;
}

#include "moc_importmailpluginmanager.cpp"
