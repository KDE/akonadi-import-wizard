/*
   SPDX-FileCopyrightText: 2012-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "abstractbase.h"
#include <PimCommonAkonadi/CreateResource>

#include <KSharedConfig>

#include <Akonadi/AgentInstanceCreateJob>
#include <Akonadi/AgentManager>
#include <Akonadi/AgentType>

#include <QMetaMethod>

using namespace Akonadi;
using namespace LibImportWizard;
AbstractBase::AbstractBase()
{
    mCreateResource = new PimCommon::CreateResource();
    connect(mCreateResource, &PimCommon::CreateResource::createResourceInfo, this, &AbstractBase::slotCreateResourceInfo);
    connect(mCreateResource, &PimCommon::CreateResource::createResourceError, this, &AbstractBase::slotCreateResourceError);
}

AbstractBase::~AbstractBase()
{
    delete mCreateResource;
}

QString LibImportWizard::AbstractBase::createResource(const QString &resources, const QString &name, const QMap<QString, QVariant> &settings)
{
    return mCreateResource->createResource(resources, name, settings);
}

void AbstractBase::setAbstractDisplayInfo(AbstractDisplayInfo *abstractDisplayInfo)
{
    mAbstractDisplayInfo = abstractDisplayInfo;
}

void AbstractBase::slotCreateResourceError(const QString &msg)
{
    addImportError(msg);
}

void AbstractBase::slotCreateResourceInfo(const QString &msg)
{
    addImportInfo(msg);
}

#include "moc_abstractbase.cpp"
