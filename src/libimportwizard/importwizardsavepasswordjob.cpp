/*
   SPDX-FileCopyrightText: 2012-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "importwizardsavepasswordjob.h"
#include "libimportwizard_debug.h"
using namespace QKeychain;

ImportWizardSavePasswordJob::ImportWizardSavePasswordJob(QObject *parent)
    : QObject(parent)
{
}

ImportWizardSavePasswordJob::~ImportWizardSavePasswordJob() = default;

bool ImportWizardSavePasswordJob::canStart() const
{
    return !mName.isEmpty() && !mPassword.isEmpty() && !mKey.isEmpty();
}

void ImportWizardSavePasswordJob::start()
{
    if (!canStart()) {
        if (mName.isEmpty()) {
            qCWarning(LIBIMPORTWIZARD_LOG) << "Error missing mName";
        }
        if (mKey.isEmpty()) {
            qCWarning(LIBIMPORTWIZARD_LOG) << "Error missing mKey";
        }
        if (mPassword.isEmpty()) {
            qCWarning(LIBIMPORTWIZARD_LOG) << "Error missing mName";
        }
        deleteLater();
        return;
    }
    auto writeJob = new WritePasswordJob(mName, this);
    connect(writeJob, &WritePasswordJob::finished, this, &ImportWizardSavePasswordJob::slotPasswordWritten);
    writeJob->setKey(mKey);
    writeJob->setTextData(mPassword);
    writeJob->start();
}

void ImportWizardSavePasswordJob::slotPasswordWritten(QKeychain::Job *baseJob)
{
    if (baseJob->error()) {
        qCWarning(LIBIMPORTWIZARD_LOG) << "Error writing password using QKeychain:" << baseJob->errorString();
    }
    deleteLater();
}

QString ImportWizardSavePasswordJob::name() const
{
    return mName;
}

void ImportWizardSavePasswordJob::setName(const QString &name)
{
    mName = name;
}

QString ImportWizardSavePasswordJob::key() const
{
    return mKey;
}

void ImportWizardSavePasswordJob::setKey(const QString &key)
{
    mKey = key;
}

QString ImportWizardSavePasswordJob::password() const
{
    return mPassword;
}

void ImportWizardSavePasswordJob::setPassword(const QString &password)
{
    mPassword = password;
}

#include "moc_importwizardsavepasswordjob.cpp"
