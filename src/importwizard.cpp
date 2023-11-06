/*
   SPDX-FileCopyrightText: 2012-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "importwizard.h"
#include "autodetect/importaddressbookpage.h"
#include "autodetect/importcalendarpage.h"
#include "autodetect/importfilterpage.h"
#include "autodetect/importfinishpage.h"
#include "autodetect/importmailpage.h"
#include "autodetect/importsettingpage.h"
#include "autodetect/selectcomponentpage.h"
#include "autodetect/selectprogrampage.h"
#include "importmailpluginmanager.h"
#include "importwizarddisplayinfo.h"
#include "importwizardkernel.h"

#include "manual/importwizardfilterinfogui.h"
#include "manual/manualimportmailpage.h"
#include "manual/manualselectfilterpage.h"

#include <MailImporter/FilterInfo>
#include <MailImporterAkonadi/FilterImporterAkonadi>

#include <Akonadi/ControlGui>
#include <KAboutData>
#include <KConfigGroup>
#include <KHelpMenu>
#include <KLocalizedString>
#include <KMessageBox>
#include <MailCommon/MailKernel>
#include <QAction>
#include <QPushButton>

ImportWizard::ImportWizard(WizardMode mode, QWidget *parent)
    : KAssistantDialog(parent)
    , mMode(mode)
{
    setModal(true);
    setWindowTitle(i18nc("@title:window", "PIM Import Tool"));
    setStandardButtons(QDialogButtonBox::Cancel | QDialogButtonBox::Help);
    mAbstractDisplayInfo = new ImportWizardDisplayInfo(this);

    auto kernel = new ImportWizardKernel(this);
    CommonKernel->registerKernelIf(kernel); // register KernelIf early, it is used by the Filter classes
    CommonKernel->registerSettingsIf(kernel); // SettingsIf is used in FolderTreeWidget
    createAutomaticModePage();
    createManualModePage();
    initializeImportModule();

    // Disable the 'next button to begin with.
    setValid(currentPage(), false);

    connect(mSelectProgramPage, &SelectProgramPage::programSelected, this, &ImportWizard::slotProgramSelected);
    connect(mSelectProgramPage, &SelectProgramPage::doubleClicked, this, &ImportWizard::slotProgramDoubleClicked);
    connect(mImportMailPage, &ImportMailPage::importMailsClicked, this, &ImportWizard::slotImportMailsClicked);
    connect(mImportFilterPage, &ImportFilterPage::importFiltersClicked, this, &ImportWizard::slotImportFiltersClicked);
    connect(mImportSettingPage, &ImportSettingPage::importSettingsClicked, this, &ImportWizard::slotImportSettingsClicked);
    connect(mImportAddressbookPage, &ImportAddressbookPage::importAddressbookClicked, this, &ImportWizard::slotImportAddressbookClicked);
    connect(mImportCalendarPage, &ImportCalendarPage::importCalendarClicked, this, &ImportWizard::slotImportCalendarClicked);

    connect(mSelectComponentPage, &SelectComponentPage::atLeastOneComponentSelected, this, &ImportWizard::slotAtLeastOneComponentSelected);

    resize(640, 480);
    Akonadi::ControlGui::widgetNeedsAkonadi(this);

    if (mode == WizardMode::AutoDetect) {
        checkModules();
        KMessageBox::information(this, i18n("Close KMail before importing data. Some plugins will modify KMail config file."));
    }
    auto helpMenu = new KHelpMenu(this, KAboutData::applicationData(), true);
    // Initialize menu
    QMenu *menu = helpMenu->menu();
    helpMenu->action(KHelpMenu::menuAboutApp)->setIcon(QIcon::fromTheme(QStringLiteral("kmail")));
    button(QDialogButtonBox::Help)->setMenu(menu);
    updatePagesFromMode();
    readConfig();
}

ImportWizard::~ImportWizard()
{
    writeConfig();
}

void ImportWizard::readConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), QStringLiteral("FolderSelectionDialog"));
    if (group.hasKey("LastSelectedFolder")) {
        mSelfilterpage->widget()->mCollectionRequestor->setCollection(CommonKernel->collectionFromId(group.readEntry("LastSelectedFolder", -1)));
    }
}

void ImportWizard::writeConfig()
{
    KConfigGroup group(KSharedConfig::openConfig(), QStringLiteral("FolderSelectionDialog"));
    group.writeEntry("LastSelectedFolder", mSelfilterpage->widget()->mCollectionRequestor->collection().id());
    group.sync();
}

void ImportWizard::updatePagesFromMode()
{
    const bool autodetectMode = (mMode == WizardMode::AutoDetect);
    setAppropriate(mSelectProgramPageItem, autodetectMode);
    setAppropriate(mSelectComponentPageItem, autodetectMode);
    setAppropriate(mImportMailPageItem, autodetectMode);
    setAppropriate(mImportFilterPageItem, autodetectMode);
    setAppropriate(mImportSettingPageItem, autodetectMode);
    setAppropriate(mImportAddressbookPageItem, autodetectMode);
    setAppropriate(mImportCalendarPageItem, autodetectMode);
    setAppropriate(mImportFinishPageItem, autodetectMode);

    setAppropriate(mSelfilterpageItem, !autodetectMode);
    setAppropriate(mImportpageItem, !autodetectMode);
    if (autodetectMode) {
        setCurrentPage(mSelectProgramPageItem);
    } else {
        setCurrentPage(mSelfilterpageItem);
    }
}

void ImportWizard::createManualModePage()
{
    mSelfilterpage = new ManualSelectFilterPage(this);
    mSelfilterpageItem = new KPageWidgetItem(mSelfilterpage, i18n("Step 1: Select Filter"));

    addPage(mSelfilterpageItem);

    mImportpage = new ManualImportMailPage(this);
    mImportpageItem = new KPageWidgetItem(mImportpage, i18n("Step 2: Importing..."));
    addPage(mImportpageItem);

    // Disable the 'next button to begin with.
    setValid(mSelfilterpageItem, false);

    connect(mSelfilterpage->widget()->mCollectionRequestor, &MailCommon::FolderRequester::folderChanged, this, &ImportWizard::slotCollectionChanged);
}

void ImportWizard::slotCollectionChanged(const Akonadi::Collection &selectedCollection)
{
    if (selectedCollection.isValid()) {
        setValid(mSelfilterpageItem, true);
    } else {
        setValid(mSelfilterpageItem, false);
    }
}

void ImportWizard::reject()
{
    if (currentPage() == mImportpageItem) {
        MailImporter::FilterInfo::terminateASAP(); // ie. import in progress
    }
    KAssistantDialog::reject();
}

void ImportWizard::createAutomaticModePage()
{
    mSelectProgramPage = new SelectProgramPage(this);
    mSelectProgramPageItem = new KPageWidgetItem(mSelectProgramPage, i18n("Detect program"));
    connect(mSelectProgramPage, &SelectProgramPage::selectManualSelectionChanged, this, &ImportWizard::slotSelectManualSelectionChanged);
    addPage(mSelectProgramPageItem);

    mSelectComponentPage = new SelectComponentPage(this);
    mSelectComponentPageItem = new KPageWidgetItem(mSelectComponentPage, i18n("Select material to import"));
    addPage(mSelectComponentPageItem);

    mImportMailPage = new ImportMailPage(this);
    mImportMailPageItem = new KPageWidgetItem(mImportMailPage, i18n("Import mail messages"));
    addPage(mImportMailPageItem);

    mImportFilterPage = new ImportFilterPage(this);
    mImportFilterPageItem = new KPageWidgetItem(mImportFilterPage, i18n("Import mail filters"));
    addPage(mImportFilterPageItem);

    mImportSettingPage = new ImportSettingPage(this);
    mImportSettingPageItem = new KPageWidgetItem(mImportSettingPage, i18n("Import settings"));
    addPage(mImportSettingPageItem);

    mImportAddressbookPage = new ImportAddressbookPage(this);
    mImportAddressbookPageItem = new KPageWidgetItem(mImportAddressbookPage, i18n("Import addressbooks"));
    addPage(mImportAddressbookPageItem);

    mImportCalendarPage = new ImportCalendarPage(this);
    mImportCalendarPageItem = new KPageWidgetItem(mImportCalendarPage, i18n("Import calendars"));
    addPage(mImportCalendarPageItem);

    mImportFinishPage = new ImportFinishPage(this);
    mImportFinishPageItem = new KPageWidgetItem(mImportFinishPage, i18n("Finish"));
    addPage(mImportFinishPageItem);
}

void ImportWizard::initializeImportModule()
{
    const QList<LibImportWizard::AbstractImporter *> lstPlugins = ImportMailPluginManager::self()->pluginsList();
    for (LibImportWizard::AbstractImporter *abstractPlugin : lstPlugins) {
        if (abstractPlugin->foundMailer()) {
            abstractPlugin->setAbstractDisplayInfo(mAbstractDisplayInfo);
            mlistImport.insert(abstractPlugin->name(), abstractPlugin);
        }
    }
}

void ImportWizard::slotProgramDoubleClicked()
{
    next();
}

void ImportWizard::slotImportAddressbookClicked()
{
    addFinishInfo(i18n("Import addressbook from %1...", mSelectedPim->name()));
    mImportAddressbookPage->setImportButtonEnabled(false);
    const bool result = mSelectedPim->importAddressBook();
    addFinishInfo(i18n("Import addressbook from %1: Done", mSelectedPim->name()));
    setValid(mImportAddressbookPageItem, result);
}

void ImportWizard::slotImportFiltersClicked()
{
    addFinishInfo(i18n("Import filters from %1...", mSelectedPim->name()));
    mImportFilterPage->setImportButtonEnabled(false);
    const bool result = mSelectedPim->importFilters();
    addFinishInfo(i18n("Import filters from %1: Done", mSelectedPim->name()));
    setValid(mImportFilterPageItem, result);
}

void ImportWizard::slotImportMailsClicked()
{
    addFinishInfo(i18n("Import mails from %1...", mSelectedPim->name()));
    mImportMailPage->setImportButtonEnabled(false);
    const bool result = mSelectedPim->importMails();
    addFinishInfo(i18n("Import mails from %1: Done", mSelectedPim->name()));
    setValid(mImportMailPageItem, result);
}

void ImportWizard::slotImportSettingsClicked()
{
    addFinishInfo(i18n("Import settings from %1...", mSelectedPim->name()));
    mImportSettingPage->setImportButtonEnabled(false);
    const bool result = mSelectedPim->importSettings();
    addFinishInfo(i18n("Import settings from %1: Done", mSelectedPim->name()));
    setValid(mImportSettingPageItem, result);
}

void ImportWizard::slotImportCalendarClicked()
{
    addFinishInfo(i18n("Import calendar from %1...", mSelectedPim->name()));
    mImportCalendarPage->setImportButtonEnabled(false);
    const bool result = mSelectedPim->importCalendar();
    addFinishInfo(i18n("Import calendar from %1: Done", mSelectedPim->name()));
    setValid(mImportCalendarPageItem, result);
}

void ImportWizard::slotProgramSelected(const QString &program)
{
    LibImportWizard::AbstractImporter *importer = mlistImport.value(program);
    if (importer) {
        mSelectedPim = importer;
        setValid(currentPage(), true);
    }
}

void ImportWizard::checkModules()
{
    mSelectProgramPage->setFoundProgram(mlistImport.keys());
}

void ImportWizard::addImportModule(LibImportWizard::AbstractImporter *import)
{
    if (import->foundMailer()) {
        mlistImport.insert(import->name(), import);
    } else {
        delete import;
    }
}

void ImportWizard::slotAtLeastOneComponentSelected(bool result)
{
    setValid(mSelectComponentPageItem, result);
}

void ImportWizard::setAppropriatePage(LibImportWizard::AbstractImporter::TypeSupportedOptions options)
{
    setAppropriate(mImportMailPageItem, (options & LibImportWizard::AbstractImporter::Mails));
    setAppropriate(mImportFilterPageItem, (options & LibImportWizard::AbstractImporter::Filters));
    setAppropriate(mImportSettingPageItem, (options & LibImportWizard::AbstractImporter::Settings));
    setAppropriate(mImportAddressbookPageItem, (options & LibImportWizard::AbstractImporter::AddressBooks));
    setAppropriate(mImportCalendarPageItem, (options & LibImportWizard::AbstractImporter::Calendars));
}

void ImportWizard::next()
{
    if (currentPage() == mSelectProgramPageItem) {
        KAssistantDialog::next();
        mSelectProgramPage->disableSelectProgram();
        mSelectComponentPage->setEnabledComponent(mSelectedPim->supportedOption());
    } else if (currentPage() == mSelectComponentPageItem) {
        setAppropriatePage(mSelectComponentPage->selectedComponents());
        KAssistantDialog::next();
        setValid(mImportMailPageItem, false);
    } else if (currentPage() == mImportMailPageItem) {
        KAssistantDialog::next();
        setValid(mImportFilterPageItem, false);
    } else if (currentPage() == mImportFilterPageItem) {
        KAssistantDialog::next();
        setValid(mImportSettingPageItem, false);
    } else if (currentPage() == mImportSettingPageItem) {
        KAssistantDialog::next();
        setValid(mImportAddressbookPageItem, false);
    } else if (currentPage() == mImportAddressbookPageItem) {
        KAssistantDialog::next();
        setValid(mImportCalendarPageItem, false);
    } else if (currentPage() == mImportCalendarPageItem) {
        KAssistantDialog::next();
        setValid(mImportFinishPageItem, true);
    } else if (currentPage() == mSelfilterpageItem) {
        // Save selected filter
        MailImporter::Filter *selectedFilter = mSelfilterpage->getSelectedFilter();
        Akonadi::Collection selectedCollection = mSelfilterpage->widget()->mCollectionRequestor->collection();
        // without filter don't go next
        if (!selectedFilter) {
            return;
        }
        // Ensure we have a valid collection.
        if (!selectedCollection.isValid()) {
            return;
        }
        // Goto next page
        KAssistantDialog::next();
        // Disable back & finish
        setValid(currentPage(), false);

        finishButton()->setEnabled(false);

        auto info = new MailImporter::FilterInfo();
        auto filterImporter = new MailImporter::FilterImporterAkonadi(info);
        auto infoGui = new ImportWizardFilterInfoGui(mImportpage, this);
        info->setFilterInfoGui(infoGui);
        info->setStatusMessage(i18n("Import in progress"));
        info->setRemoveDupMessage(mSelfilterpage->removeDupMsg_checked());
        info->clear(); // Clear info from last time
        filterImporter->setRootCollection(selectedCollection);
        selectedFilter->setFilterImporter(filterImporter);
        selectedFilter->setFilterInfo(info);
        selectedFilter->import();
        info->setStatusMessage(i18n("Import finished"));
        // Cleanup
        delete filterImporter;
        delete info;
        // Enable finish & back buttons
        setValid(currentPage(), true);
        finishButton()->setEnabled(true);
    } else {
        KAssistantDialog::next();
    }
}

void ImportWizard::enableAllImportButton()
{
    mImportMailPage->setImportButtonEnabled(true);
    mImportFilterPage->setImportButtonEnabled(true);
    mImportSettingPage->setImportButtonEnabled(true);
    mImportAddressbookPage->setImportButtonEnabled(true);
    mImportCalendarPage->setImportButtonEnabled(true);
}

void ImportWizard::back()
{
    if (currentPage() == mSelectProgramPageItem) {
        return;
    } else if (currentPage() == mImportFilterPageItem || currentPage() == mImportSettingPageItem || currentPage() == mImportAddressbookPageItem
               || currentPage() == mImportCalendarPageItem || currentPage() == mImportFinishPageItem) {
        enableAllImportButton();
    }
    KAssistantDialog::back();
}

ImportMailPage *ImportWizard::importMailPage() const
{
    return mImportMailPage;
}

ImportFilterPage *ImportWizard::importFilterPage() const
{
    return mImportFilterPage;
}

ImportAddressbookPage *ImportWizard::importAddressBookPage() const
{
    return mImportAddressbookPage;
}

ImportSettingPage *ImportWizard::importSettingPage() const
{
    return mImportSettingPage;
}

ImportFinishPage *ImportWizard::importFinishPage() const
{
    return mImportFinishPage;
}

ImportCalendarPage *ImportWizard::importCalendarPage() const
{
    return mImportCalendarPage;
}

void ImportWizard::addFinishInfo(const QString &log)
{
    mImportFinishPage->addImportInfo(log);
}

void ImportWizard::addFinishError(const QString &log)
{
    mImportFinishPage->addImportError(log);
}

void ImportWizard::slotSelectManualSelectionChanged(bool b)
{
    mMode = b ? WizardMode::Manual : WizardMode::AutoDetect;
    updatePagesFromMode();
}

#include "moc_importwizard.cpp"
