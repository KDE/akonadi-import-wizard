/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef SELECTCOMPONENTPAGE_H
#define SELECTCOMPONENTPAGE_H

#include <QWidget>
#include "abstractimporter.h"

namespace Ui {
class SelectComponentPage;
}

class SelectComponentPage : public QWidget
{
    Q_OBJECT

public:
    explicit SelectComponentPage(QWidget *parent = nullptr);
    ~SelectComponentPage();

    void setEnabledComponent(LibImportWizard::AbstractImporter::TypeSupportedOptions options);
    LibImportWizard::AbstractImporter::TypeSupportedOptions selectedComponents() const;

Q_SIGNALS:
    void atLeastOneComponentSelected(bool componentSelected);

private:
    void slotEverythingClicked(bool clicked);
    void slotComponentClicked();
    LibImportWizard::AbstractImporter::TypeSupportedOptions mOptions;
    Ui::SelectComponentPage *const ui;
};

#endif // SELECTCOMPONENTPAGE_H
