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


#include "importmailpluginmanager.h"
#include "importwizard_debug.h"
#include "abstractimporter.h"

#include <KPluginMetaData>
#include <KPluginLoader>
#include <KPluginFactory>

#include <QFileInfo>
#include <QVector>

Q_GLOBAL_STATIC(ImportMailPluginManager, s_instance)


namespace
{
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
    return s_instance;
}

bool ImportMailPluginManager::initializePluginList()
{
    const QVector<KPluginMetaData> plugins = KPluginLoader::findPlugins(QStringLiteral("importwizard"), [](const KPluginMetaData & md) {
        return md.serviceTypes().contains(QStringLiteral("ImportWizard/PluginMailImporter"));
    });

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
        item->plugin = pluginLoader.factory()->create<AbstractImporter>(this, QVariantList() << item->metaDataFileNameBaseName);
        mPluginDataList.append(item->pluginData);
    }
}

QVector<AbstractImporter *> ImportMailPluginManager::pluginsList() const
{
    QVector<AbstractImporter *> lst;
    QVector<ImportMailPluginManagerInfo>::ConstIterator end(mPluginList.constEnd());
    for (QVector<ImportMailPluginManagerInfo>::ConstIterator it = mPluginList.constBegin(); it != end; ++it) {
        if ((*it).plugin) {
            lst << (*it).plugin;
        }
    }
    return lst;
}

PluginUtilData ImportMailPluginManager::createPluginMetaData(const KPluginMetaData &metaData)
{
    PluginUtilData pluginData;
    pluginData.mDescription = metaData.description();
    pluginData.mName = metaData.name();
    pluginData.mIdentifier = metaData.pluginId();
    return pluginData;
}
