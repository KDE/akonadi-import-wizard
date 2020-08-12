/*
   SPDX-FileCopyrightText: 2003 Laurence Anderson <l.d.anderson@warwick.ac.uk>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef ManualImportMailPage_H
#define ManualImportMailPage_H

#include "ui_manualimportmailpage.h"

class ManualImportMailPage : public QWidget
{
    Q_OBJECT
public:
    explicit ManualImportMailPage(QWidget *parent = nullptr);
    ~ManualImportMailPage();

    Ui::ManualImportMailPage *widget() const;
private:
    Ui::ManualImportMailPage *mWidget = nullptr;
};

#endif
