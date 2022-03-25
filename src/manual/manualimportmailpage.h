/*
   SPDX-FileCopyrightText: 2003 Laurence Anderson <l.d.anderson@warwick.ac.uk>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include "ui_manualimportmailpage.h"

class ManualImportMailPage : public QWidget
{
    Q_OBJECT
public:
    explicit ManualImportMailPage(QWidget *parent = nullptr);
    ~ManualImportMailPage() override;

    Ui::ManualImportMailPage *widget() const;

private:
    Ui::ManualImportMailPage *mWidget = nullptr;
};
