/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef IMPORTSETTINGPAGE_H
#define IMPORTSETTINGPAGE_H

#include <QWidget>

namespace Ui {
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
    Ui::ImportSettingPage *ui = nullptr;
};

#endif // IMPORTSETTINGPAGE_H
