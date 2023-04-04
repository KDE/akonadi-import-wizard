/*
   SPDX-FileCopyrightText: 2003 Laurence Anderson <l.d.anderson@warwick.ac.uk>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "ui_manualselectfilterpage.h"
#include <QList>
namespace MailImporter
{
class Filter;
}

class ManualSelectFilterPage : public QWidget
{
    Q_OBJECT
public:
    explicit ManualSelectFilterPage(QWidget *parent = nullptr);
    ~ManualSelectFilterPage() override;

    void addFilter(MailImporter::Filter *f);
    MailImporter::Filter *getSelectedFilter() const;
    bool removeDupMsg_checked() const;

    Ui::ManualSelectFilterPage *widget() const;

private Q_SLOTS:
    void filterSelected(int i);

private:
    Ui::ManualSelectFilterPage *mWidget = nullptr;
    QList<MailImporter::Filter *> mFilterList;
};
