/*
   SPDX-FileCopyrightText: 2017-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#pragma once

#include <KPluginMetaData>
#include <QList>
#include <QObject>

namespace LibImportWizard
{
class AbstractImporter;
}

class PluginUtilData
{
public:
    PluginUtilData() = default;

    QStringList mExtraInfo;
    QString mIdentifier;
    QString mName;
};

class ImportMailPluginManagerInfo
{
public:
    ImportMailPluginManagerInfo() = default;

    QString metaDataFileName;
    PluginUtilData pluginData;
    KPluginMetaData data;
    LibImportWizard::AbstractImporter *plugin = nullptr;
};

class ImportMailPluginManager : public QObject
{
    Q_OBJECT
public:
    explicit ImportMailPluginManager(QObject *parent = nullptr);
    ~ImportMailPluginManager() override;

    static ImportMailPluginManager *self();
    bool initializePluginList();

    QList<LibImportWizard::AbstractImporter *> pluginsList() const;

private:
    void loadPlugin(ImportMailPluginManagerInfo *item);
    PluginUtilData createPluginMetaData(const KPluginMetaData &metaData);
    QList<ImportMailPluginManagerInfo> mPluginList;
    QList<PluginUtilData> mPluginDataList;
};
