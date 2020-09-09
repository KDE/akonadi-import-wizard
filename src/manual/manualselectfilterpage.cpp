/*
   SPDX-FileCopyrightText: 2003 Laurence Anderson <l.d.anderson@warwick.ac.uk>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

// Local includes
#include "manualselectfilterpage.h"

// Filter includes
#include <filtermbox.h>
#include <filteroe.h>
#include <filterpmail.h>
#include <filterplain.h>
#include <filterevolution.h>
#include <filtermailapp.h>
#include <filterevolution_v2.h>
#include <filteropera.h>
#include <filterthunderbird.h>
#include <filterkmail_maildir.h>
#include <filterkmailarchive.h>
#include <filtersylpheed.h>
#include <filterthebat.h>
#include <filterlnotes.h>
#include <filtermailmangzip.h>
#include <filtericedove.h>

#include <filters.h>

#include <filterevolution_v3.h>

// KDE includes

#include <KLocalizedString>

// Qt includes
#include <QCheckBox>

#include <QStandardPaths>

using namespace MailImporter;

ManualSelectFilterPage::ManualSelectFilterPage(QWidget *parent)
    : QWidget(parent)
    , mWidget(new Ui::ManualSelectFilterPage)
{
    mWidget->setupUi(this);
    mWidget->mIntroSidebar->setPixmap(QStandardPaths::locate(QStandardPaths::GenericDataLocation, QStringLiteral("importwizard/pics/step1.png")));
    connect(mWidget->mFilterCombo, QOverload<int>::of(&QComboBox::activated), this, &ManualSelectFilterPage::filterSelected);

    addFilter(new MailImporter::FilterKMailArchive);
    addFilter(new MailImporter::FilterMBox);
    addFilter(new MailImporter::FilterEvolution);
    addFilter(new MailImporter::FilterEvolution_v2);
    addFilter(new MailImporter::FilterEvolution_v3);
    addFilter(new MailImporter::FilterKMail_maildir);
    addFilter(new MailImporter::FilterMailApp);
    addFilter(new MailImporter::FilterOpera);
    addFilter(new MailImporter::FilterSylpheed);
    addFilter(new MailImporter::FilterThunderbird);
    addFilter(new MailImporter::FilterIcedove);
    addFilter(new MailImporter::FilterTheBat);
    addFilter(new MailImporter::FilterOE);
    addFilter(new MailImporter::FilterPMail);
    addFilter(new MailImporter::FilterLNotes);
    addFilter(new MailImporter::FilterPlain);
    addFilter(new MailImporter::FilterMailmanGzip);

    // Ensure we return the correct type of Akonadi collection.
    mWidget->mCollectionRequestor->setMustBeReadWrite(true);
}

ManualSelectFilterPage::~ManualSelectFilterPage()
{
    qDeleteAll(mFilterList);
    mFilterList.clear();
    delete mWidget;
}

void ManualSelectFilterPage::filterSelected(int i)
{
    QString info = mFilterList.at(i)->info();
    const QString author = mFilterList.at(i)->author();
    if (!author.isEmpty()) {
        info += i18n("<p><i>Written by %1.</i></p>", author);
    }
    mWidget->mDesc->setText(info);
}

void ManualSelectFilterPage::addFilter(Filter *f)
{
    mFilterList.append(f);
    mWidget->mFilterCombo->addItem(f->name());
    if (mWidget->mFilterCombo->count() == 1) {
        filterSelected(0);    // Setup description box with fist filter selected
    }
}

bool ManualSelectFilterPage::removeDupMsg_checked() const
{
    return mWidget->remDupMsg->isChecked();
}

Filter *ManualSelectFilterPage::getSelectedFilter() const
{
    return mFilterList.at(mWidget->mFilterCombo->currentIndex());
}

Ui::ManualSelectFilterPage *ManualSelectFilterPage::widget() const
{
    return mWidget;
}
