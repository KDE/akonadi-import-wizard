/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QWidget>

namespace Ui
{
class ImportFilterPage;
}

class ImportFilterPage : public QWidget
{
    Q_OBJECT
public:
    explicit ImportFilterPage(QWidget *parent = nullptr);
    ~ImportFilterPage();

    void addImportInfo(const QString &log);
    void addImportError(const QString &log);
    void setImportButtonEnabled(bool enabled);

Q_SIGNALS:
    void importFiltersClicked();

private:
    Ui::ImportFilterPage *const ui;
};

