/*
   Copyright (C) 2012-2018 Montel Laurent <montel@kde.org>

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
#ifndef IMPORTWIZARD_H
#define IMPORTWIZARD_H

#include "abstractimporter.h"
#include "libimportwizard_export.h"

#include <KAssistantDialog>
namespace LibImportWizard {
class AbstractImporter;
class AbstractDisplayInfo;
}
class KPageWidgetItem;
class SelectProgramPage;
class SelectComponentPage;
class ImportMailPage;
class ImportFilterPage;
class ImportSettingPage;
class ImportAddressbookPage;
class ImportFinishPage;
class ImportCalendarPage;
class ManualSelectFilterPage;
class ManualImportMailPage;

namespace Akonadi {
class Collection;
}
class LIBIMPORTWIZARD_EXPORT ImportWizard : public KAssistantDialog
{
    Q_OBJECT
public:
    enum class WizardMode {
        AutoDetect = 0,
        Manual = 1
    };

    explicit ImportWizard(WizardMode mode, QWidget *parent = nullptr);
    ~ImportWizard();

    void next() override;
    void reject() override;
    void back() override;

    ImportMailPage *importMailPage() const;
    ImportFilterPage *importFilterPage() const;
    ImportAddressbookPage *importAddressBookPage() const;
    ImportSettingPage *importSettingPage() const;
    ImportFinishPage *importFinishPage() const;
    ImportCalendarPage *importCalendarPage() const;

private Q_SLOTS:
    void slotProgramSelected(const QString &program);
    void slotImportMailsClicked();
    void slotImportFiltersClicked();
    void slotProgramDoubleClicked();
    void slotAtLeastOneComponentSelected(bool b);
    void slotImportSettingsClicked();
    void slotImportAddressbookClicked();
    void slotImportCalendarClicked();
    void slotSelectManualSelectionChanged(bool b);
    void slotCollectionChanged(const Akonadi::Collection &selectedCollection);

private:
    void addFinishInfo(const QString &log);
    void addFinishError(const QString &log);

    void readConfig();
    void writeConfig();
    void initializeImportModule();
    void createAutomaticModePage();
    void addImportModule(LibImportWizard::AbstractImporter *);
    void checkModules();
    void setAppropriatePage(LibImportWizard::AbstractImporter::TypeSupportedOptions options);
    void enableAllImportButton();
    void createManualModePage();
    void updatePagesFromMode();

    QMap<QString, LibImportWizard::AbstractImporter *> mlistImport;

    WizardMode mMode;
    LibImportWizard::AbstractImporter *mSelectedPim = nullptr;

    KPageWidgetItem *mSelectProgramPageItem = nullptr;
    KPageWidgetItem *mSelectComponentPageItem = nullptr;
    KPageWidgetItem *mImportMailPageItem = nullptr;
    KPageWidgetItem *mImportFilterPageItem = nullptr;
    KPageWidgetItem *mImportSettingPageItem = nullptr;
    KPageWidgetItem *mImportAddressbookPageItem = nullptr;
    KPageWidgetItem *mImportCalendarPageItem = nullptr;
    KPageWidgetItem *mImportFinishPageItem = nullptr;

    SelectProgramPage *mSelectProgramPage = nullptr;
    SelectComponentPage *mSelectComponentPage = nullptr;
    ImportMailPage *mImportMailPage = nullptr;
    ImportFilterPage *mImportFilterPage = nullptr;
    ImportSettingPage *mImportSettingPage = nullptr;
    ImportAddressbookPage *mImportAddressbookPage = nullptr;
    ImportFinishPage *mImportFinishPage = nullptr;
    ImportCalendarPage *mImportCalendarPage = nullptr;

    ManualSelectFilterPage *mSelfilterpage = nullptr;
    ManualImportMailPage *mImportpage = nullptr;
    KPageWidgetItem *mSelfilterpageItem = nullptr;
    KPageWidgetItem *mImportpageItem = nullptr;

    LibImportWizard::AbstractDisplayInfo *mAbstractDisplayInfo = nullptr;
};

#endif /* IMPORTWIZARD_H */
