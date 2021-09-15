/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "importwizardkernel.h"

#include <Akonadi/ChangeRecorder>
#include <Akonadi/EntityMimeTypeFilterModel>
#include <Akonadi/EntityTreeModel>
#include <Akonadi/Session>
#include <KIdentityManagement/IdentityManager>
#include <KSharedConfig>
#include <MailCommon/FolderCollectionMonitor>

ImportWizardKernel::ImportWizardKernel(QObject *parent)
    : QObject(parent)
{
    mIdentityManager = KIdentityManagement::IdentityManager::self();
    auto session = new Akonadi::Session("ImportWizard Kernel ETM", this);
    mFolderCollectionMonitor = new MailCommon::FolderCollectionMonitor(session, this);

    mEntityTreeModel = new Akonadi::EntityTreeModel(folderCollectionMonitor(), this);
    mEntityTreeModel->setListFilter(Akonadi::CollectionFetchScope::Enabled);
    mEntityTreeModel->setItemPopulationStrategy(Akonadi::EntityTreeModel::LazyPopulation);

    mCollectionModel = new Akonadi::EntityMimeTypeFilterModel(this);
    mCollectionModel->setSourceModel(mEntityTreeModel);
    mCollectionModel->addMimeTypeInclusionFilter(Akonadi::Collection::mimeType());
    mCollectionModel->setHeaderGroup(Akonadi::EntityTreeModel::CollectionTreeHeaders);
    mCollectionModel->setDynamicSortFilter(true);
    mCollectionModel->setSortCaseSensitivity(Qt::CaseInsensitive);
}

KIdentityManagement::IdentityManager *ImportWizardKernel::identityManager()
{
    return mIdentityManager;
}

MessageComposer::MessageSender *ImportWizardKernel::msgSender()
{
    Q_ASSERT(false);
    return nullptr;
}

Akonadi::EntityMimeTypeFilterModel *ImportWizardKernel::collectionModel() const
{
    return mCollectionModel;
}

KSharedConfig::Ptr ImportWizardKernel::config()
{
    return KSharedConfig::openConfig();
}

void ImportWizardKernel::syncConfig()
{
    Q_ASSERT(false);
}

MailCommon::JobScheduler *ImportWizardKernel::jobScheduler() const
{
    Q_ASSERT(false);
    return nullptr;
}

Akonadi::ChangeRecorder *ImportWizardKernel::folderCollectionMonitor() const
{
    return mFolderCollectionMonitor->monitor();
}

void ImportWizardKernel::updateSystemTray()
{
    Q_ASSERT(false);
}

bool ImportWizardKernel::showPopupAfterDnD()
{
    return false;
}

qreal ImportWizardKernel::closeToQuotaThreshold()
{
    return 80;
}

QStringList ImportWizardKernel::customTemplates()
{
    Q_ASSERT(false);
    return QStringList();
}

bool ImportWizardKernel::excludeImportantMailFromExpiry()
{
    Q_ASSERT(false);
    return true;
}

Akonadi::Collection::Id ImportWizardKernel::lastSelectedFolder()
{
    Q_ASSERT(false);
    return Akonadi::Collection::Id();
}

void ImportWizardKernel::setLastSelectedFolder(Akonadi::Collection::Id col)
{
    Q_UNUSED(col)
}

void ImportWizardKernel::expunge(Akonadi::Collection::Id col, bool sync)
{
    Q_UNUSED(col)
    Q_UNUSED(sync)
}
