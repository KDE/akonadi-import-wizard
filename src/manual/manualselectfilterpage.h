/*
   SPDX-FileCopyrightText: 2003 Laurence Anderson <l.d.anderson@warwick.ac.uk>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef MANUALSELECTFILTERPAGE_H
#define MANUALSELECTFILTERPAGE_H

#include "ui_manualselectfilterpage.h"
#include <QVector>
namespace MailImporter {
class Filter;
}

class ManualSelectFilterPage : public QWidget
{
    Q_OBJECT
public:
    explicit ManualSelectFilterPage(QWidget *parent = nullptr);
    ~ManualSelectFilterPage();

    void  addFilter(MailImporter::Filter *f);
    MailImporter::Filter *getSelectedFilter() const;
    bool removeDupMsg_checked() const;

    Ui::ManualSelectFilterPage *widget() const;

private Q_SLOTS:
    void filterSelected(int i);

private:
    Ui::ManualSelectFilterPage *mWidget = nullptr;
    QVector<MailImporter::Filter *> mFilterList;
};

#endif
