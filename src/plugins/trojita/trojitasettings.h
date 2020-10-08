/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef TrojitaSettings_H
#define TrojitaSettings_H

#include "abstractsettings.h"

class QSettings;

class TrojitaSettings : public AbstractSettings
{
public:
    explicit TrojitaSettings(const QString &filename);
    ~TrojitaSettings();
    void importSettings();
private:
    void readImapAccount();
    void readIdentity();
    void readGlobalSettings();
    void readTransport();
    QSettings *settings = nullptr;
};

#endif // ClawsMailSettings_H
