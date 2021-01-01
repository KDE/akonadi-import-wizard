/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ABSTRACTBASE_H
#define ABSTRACTBASE_H

#include <QObject>
#include "libimportwizard_export.h"
#include <QMap>
#include <QVariant>

namespace PimCommon {
class CreateResource;
}

namespace LibImportWizard {
class AbstractDisplayInfo;
class LIBIMPORTWIZARD_EXPORT AbstractBase : public QObject
{
    Q_OBJECT
public:
    explicit AbstractBase();
    virtual ~AbstractBase();

    QString createResource(const QString &resources, const QString &name, const QMap<QString, QVariant> &settings);

    void setAbstractDisplayInfo(AbstractDisplayInfo *abstractDisplayInfo);

protected:
    virtual void addImportInfo(const QString &log) = 0;
    virtual void addImportError(const QString &log) = 0;

    AbstractDisplayInfo *mAbstractDisplayInfo = nullptr;

private:
    void slotCreateResourceError(const QString &);
    void slotCreateResourceInfo(const QString &);
    PimCommon::CreateResource *mCreateResource = nullptr;
};
}
#endif // ABSTRACTBASE_H
