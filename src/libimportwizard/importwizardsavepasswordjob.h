/*
   SPDX-FileCopyrightText: 2012-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QObject>
#include <qt6keychain/keychain.h>
class ImportWizardSavePasswordJob : public QObject
{
    Q_OBJECT
public:
    explicit ImportWizardSavePasswordJob(QObject *parent = nullptr);
    ~ImportWizardSavePasswordJob() override;

    [[nodiscard]] bool canStart() const;

    void start();

    [[nodiscard]] QString password() const;
    void setPassword(const QString &password);

    [[nodiscard]] QString key() const;
    void setKey(const QString &key);

    [[nodiscard]] QString name() const;
    void setName(const QString &name);

private:
    void slotPasswordWritten(QKeychain::Job *baseJob);
    QString mPassword;
    QString mKey;
    QString mName;
};
