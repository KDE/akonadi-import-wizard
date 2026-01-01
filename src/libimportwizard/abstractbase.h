/*
   SPDX-FileCopyrightText: 2012-2026 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "libimportwizard_export.h"
#include <QMap>
#include <QObject>
#include <QVariant>

namespace PimCommon
{
class CreateResource;
}

namespace LibImportWizard
{
class AbstractDisplayInfo;
class LIBIMPORTWIZARD_EXPORT AbstractBase : public QObject
{
    Q_OBJECT
public:
    explicit AbstractBase();
    ~AbstractBase() override;

    QString createResource(const QString &resources, const QString &name, const QMap<QString, QVariant> &settings);

    void setAbstractDisplayInfo(AbstractDisplayInfo *abstractDisplayInfo);

protected:
    virtual void addImportInfo(const QString &log) = 0;
    virtual void addImportError(const QString &log) = 0;

    AbstractDisplayInfo *mAbstractDisplayInfo = nullptr;

private:
    LIBIMPORTWIZARD_NO_EXPORT void slotCreateResourceError(const QString &);
    LIBIMPORTWIZARD_NO_EXPORT void slotCreateResourceInfo(const QString &);
    PimCommon::CreateResource *mCreateResource = nullptr;
};
}
