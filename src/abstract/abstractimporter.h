/*
   Copyright (C) 2012-2017 Montel Laurent <montel@kde.org>

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public
   License as published by the Free Software Foundation; either
   version 2 of the License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; see the file COPYING.  If not, write to
   the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.
*/

#ifndef AbstractImporter_H
#define AbstractImporter_H
#include "MailCommon/FilterImporterExporter"
#include "libimportwizard_export.h"

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

namespace LibImportWizard {
class AbstractDisplayInfo;
class LIBIMPORTWIZARD_EXPORT AbstractImporter : public QObject
{
    Q_OBJECT
public:
    enum TypeSupportedOption {
        None = 0,
        Mails = 1,
        Settings = 2,
        Filters = 4,
        AddressBooks = 8,
        Calendars = 16
    };

    Q_DECLARE_FLAGS(TypeSupportedOptions, TypeSupportedOption)

    explicit AbstractImporter(QObject *parent);
    virtual ~AbstractImporter();

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

    void setParentWidget(QWidget *parent);
    QWidget *parentWidget() const;

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
    AbstractDisplayInfo *mAbstractDisplayInfo;
    QWidget *mParentWidget;
};
}

#endif /* AbstractImporter_H */

