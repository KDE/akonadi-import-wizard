/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include "libimportwizard_export.h"
#include <QColor>
#include <QString>
#include <QUrl>

struct LIBIMPORTWIZARD_EXPORT ldapStruct {
    ldapStruct() = default;

    QUrl ldapUrl;
    QString dn;
    QString saslMech;
    QString fileName;
    QString description;
    QString password;
    int maxHint = -1;
    int port = -1;
    int limit = -1;
    int timeout = -1;
    bool useSSL = false;
    bool useTLS = false;
};

struct tagStruct {
    QString name;
    QColor color;
};
Q_DECLARE_TYPEINFO(tagStruct, Q_MOVABLE_TYPE);

namespace ImportWizardUtil
{
enum ResourceType { Imap, Pop3, Ldap };

LIBIMPORTWIZARD_EXPORT void mergeLdap(const ldapStruct &ldap);
LIBIMPORTWIZARD_EXPORT void addAkonadiTag(const QVector<tagStruct> &tagList);
void storePassword(const QString &name, ImportWizardUtil::ResourceType type, const QString &password);
}

