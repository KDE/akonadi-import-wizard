/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef BalsaSettings_H
#define BalsaSettings_H

#include "abstractsettings.h"

class KConfigGroup;

class BalsaSettings : public AbstractSettings
{
public:
    explicit BalsaSettings(const QString &filename);
    ~BalsaSettings();

    void importSettings();

private:
    void readAccount(const KConfigGroup &grp, bool autoCheck, int autoDelay);
    void readIdentity(const KConfigGroup &grp);
    void readTransport(const KConfigGroup &grp);
    void readGlobalSettings(const KConfig &config);

    QHash<QString, QString> mHashSmtp;
    QString mFileName;
};

#endif // BalsaSettings_H
