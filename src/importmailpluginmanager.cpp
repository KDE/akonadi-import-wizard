/*
   SPDX-FileCopyrightText: 2017-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "importmailpluginmanager.h"
#include "importwizard_debug.h"
#include "abstractimporter.h"

#include <KPluginMetaData>
#include <KPluginLoader>
#include <KPluginFactory>

#include <QFileInfo>

namespace {
QString pluginVersion()
{
    return QStringLiteral("1.0");
}
}

ImportMailPluginManager::ImportMailPluginManager(QObject *parent)
    : QObject(parent)
{
    initializePluginList();
}

ImportMailPluginManager::~ImportMailPluginManager()
{
}

ImportMailPluginManager *ImportMailPluginManager::self()
{
    static ImportMailPluginManager s_self;
    return &s_self;
}

bool ImportMailPluginManager::initializePluginList()
{
    const QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(QStringLiteral("importwizard"));

    QVectorIterator<KPluginMetaData> i(plugins);
    i.toBack();
    QSet<QString> unique;
    while (i.hasPrevious()) {
        ImportMailPluginManagerInfo info;
        const KPluginMetaData data = i.previous();

        //1) get plugin data => name/description etc.
        info.pluginData = createPluginMetaData(data);
        //2) look at if plugin is activated
        info.metaDataFileNameBaseName = QFileInfo(data.fileName()).baseName();
        info.metaDataFileName = data.fileName();
        if (pluginVersion() == data.version()) {
            // only load plugins once, even if found multiple times!
            if (unique.contains(info.metaDataFileNameBaseName)) {
                continue;
            }
            info.plugin = nullptr;
            mPluginList.push_back(info);
            unique.insert(info.metaDataFileNameBaseName);
        } else {
            qCWarning(IMPORTWIZARD_LOG) << "Plugin " << data.name() << " doesn't have correction plugin version. It will not be loaded.";
        }
    }
    QVector<ImportMailPluginManagerInfo>::iterator end(mPluginList.end());
    for (QVector<ImportMailPluginManagerInfo>::iterator it = mPluginList.begin(); it != end; ++it) {
        loadPlugin(&(*it));
    }
    return true;
}

void ImportMailPluginManager::loadPlugin(ImportMailPluginManagerInfo *item)
{
    KPluginLoader pluginLoader(item->metaDataFileName);
    if (pluginLoader.factory()) {
        item->plugin = pluginLoader.factory()->create<LibImportWizard::AbstractImporter>(this, QVariantList() << item->metaDataFileNameBaseName);
        mPluginDataList.append(item->pluginData);
    }
}

QVector<LibImportWizard::AbstractImporter *> ImportMailPluginManager::pluginsList() const
{
    QVector<LibImportWizard::AbstractImporter *> lst;
    QVector<ImportMailPluginManagerInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<ImportMailPluginManagerInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
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
