/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef IMPORTWIZARDSAVEPASSWORDJOB_H
#define IMPORTWIZARDSAVEPASSWORDJOB_H

#include <QObject>
#include "libimportwizard_export.h"
#include <qt5keychain/keychain.h>
class LIBIMPORTWIZARD_EXPORT ImportWizardSavePasswordJob : public QObject
{
    Q_OBJECT
public:
    explicit ImportWizardSavePasswordJob(QObject *parent = nullptr);
    ~ImportWizardSavePasswordJob();

    Q_REQUIRED_RESULT bool canStart() const;

    void start();

    Q_REQUIRED_RESULT QString password() const;
    void setPassword(const QString &password);

    Q_REQUIRED_RESULT QString key() const;
    void setKey(const QString &key);

    Q_REQUIRED_RESULT QString name() const;
    void setName(const QString &name);

private:
    void slotPasswordWritten(QKeychain::Job *baseJob);
    QString mPassword;
    QString mKey;
    QString mName;
};

#endif // IMPORTWIZARDSAVEPASSWORDJOB_H
