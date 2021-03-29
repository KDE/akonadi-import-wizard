/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QWidget>

namespace Ui
{
class ImportSettingPage;
}

class ImportSettingPage : public QWidget
{
    Q_OBJECT
public:
    explicit ImportSettingPage(QWidget *parent = nullptr);
    ~ImportSettingPage();
    void addImportInfo(const QString &log);
    void addImportError(const QString &log);
    void setImportButtonEnabled(bool enabled);

Q_SIGNALS:
    void importSettingsClicked();

private:
    Ui::ImportSettingPage *const ui;
};

