/*
   SPDX-FileCopyrightText: 2017-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef IMPORTMAILPLUGINMANAGER_H
#define IMPORTMAILPLUGINMANAGER_H

#include <QObject>
#include <QVector>
#include <KPluginMetaData>

namespace LibImportWizard {
class AbstractImporter;
}

class PluginUtilData
{
public:
    PluginUtilData()
    {
    }

    QStringList mExtraInfo;
    QString mIdentifier;
    QString mName;
};

class ImportMailPluginManagerInfo
{
public:
    ImportMailPluginManagerInfo()
    {
    }

    QString metaDataFileNameBaseName;
    QString metaDataFileName;
    PluginUtilData pluginData;
    LibImportWizard::AbstractImporter *plugin = nullptr;
};

class ImportMailPluginManager : public QObject
{
    Q_OBJECT
public:
    explicit ImportMailPluginManager(QObject *parent = nullptr);
    ~ImportMailPluginManager();

    static ImportMailPluginManager *self();
    bool initializePluginList();

    QVector<LibImportWizard::AbstractImporter *> pluginsList() const;

private:
    void loadPlugin(ImportMailPluginManagerInfo *item);
    PluginUtilData createPluginMetaData(const KPluginMetaData &metaData);
    QVector<ImportMailPluginManagerInfo> mPluginList;
    QVector<PluginUtilData> mPluginDataList;
};

#endif // IMPORTMAILPLUGINMANAGER_H
