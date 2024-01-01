/*
   SPDX-FileCopyrightText: 2012-2024 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once
#include "libimportwizard_export.h"
#include <MailCommon/FilterImporterExporter>
#include <QList>
namespace MailImporter
{
class FilterInfo;
class Filter;
}

namespace MailCommon
{
class MailFilter;
class FilterImporterExporter;
}

namespace LibImportWizard
{
class AbstractDisplayInfo;
class LIBIMPORTWIZARD_EXPORT AbstractImporter : public QObject
{
    Q_OBJECT
public:
    enum TypeSupportedOption { None = 0, Mails = 1, Settings = 2, Filters = 4, AddressBooks = 8, Calendars = 16 };

    Q_DECLARE_FLAGS(TypeSupportedOptions, TypeSupportedOption)

    explicit AbstractImporter(QObject *parent);
    ~AbstractImporter() override;

    /**
     * Return true if mail found on system
     */
    virtual bool foundMailer() const = 0;

    /**
     * Return type of data that we can import
     */
    virtual TypeSupportedOptions supportedOption() = 0;
    /**
     * Return name for plugins
     */
    virtual QString name() const = 0;

    virtual bool importSettings();
    virtual bool importMails();
    virtual bool importFilters();
    virtual bool importAddressBook();
    virtual bool importCalendar();

    void setAbstractDisplayInfo(AbstractDisplayInfo *info);

protected:
    void initializeFilter(MailImporter::Filter &filter);

    void appendFilters(const QList<MailCommon::MailFilter *> &filters);
    void addImportFilterInfo(const QString &log) const;
    void addImportFilterError(const QString &log) const;
    bool addFilters(const QString &filterPath, MailCommon::FilterImporterExporter::FilterType type);
    void addImportSettingsInfo(const QString &log) const;
    void addImportCalendarInfo(const QString &log) const;

    QString mPath;
    AbstractDisplayInfo *mAbstractDisplayInfo = nullptr;
    QWidget *mParentWidget = nullptr;
};
}
