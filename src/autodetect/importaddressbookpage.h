/*
   SPDX-FileCopyrightText: 2012-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef IMPORTADDRESSBOOKPAGE_H
#define IMPORTADDRESSBOOKPAGE_H

#include <QWidget>

namespace Ui {
class ImportAddressbookPage;
}

class ImportAddressbookPage : public QWidget
{
    Q_OBJECT

public:
    explicit ImportAddressbookPage(QWidget *parent = nullptr);
    ~ImportAddressbookPage();

    void addImportInfo(const QString &log);
    void addImportError(const QString &log);
    void setImportButtonEnabled(bool enabled);

Q_SIGNALS:
    void importAddressbookClicked();

private:
    Ui::ImportAddressbookPage *const ui;
};

#endif // IMPORTADDRESSBOOKPAGE_H
