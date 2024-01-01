/*
   SPDX-FileCopyrightText: 2012-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "abstractsettings.h"

class QSettings;

class TrojitaSettings : public AbstractSettings
{
public:
    explicit TrojitaSettings(const QString &filename);
    ~TrojitaSettings() override;
    void importSettings();

private:
    void readImapAccount();
    void readIdentity();
    void readGlobalSettings();
    void readTransport();
    QSettings *settings = nullptr;
};
