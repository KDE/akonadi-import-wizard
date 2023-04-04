/*
   SPDX-FileCopyrightText: 2012-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "libimportwizard_export.h"
#include <QObject>
#include <qt6keychain/keychain.h>
class LIBIMPORTWIZARD_EXPORT ImportWizardSavePasswordJob : public QObject
{
    Q_OBJECT
public:
    explicit ImportWizardSavePasswordJob(QObject *parent = nullptr);
    ~ImportWizardSavePasswordJob() override;

    Q_REQUIRED_RESULT bool canStart() const;

    void start();

    Q_REQUIRED_RESULT QString password() const;
    void setPassword(const QString &password);

    Q_REQUIRED_RESULT QString key() const;
    void setKey(const QString &key);

    Q_REQUIRED_RESULT QString name() const;
    void setName(const QString &name);

private:
    LIBIMPORTWIZARD_NO_EXPORT void slotPasswordWritten(QKeychain::Job *baseJob);
    QString mPassword;
    QString mKey;
    QString mName;
};
