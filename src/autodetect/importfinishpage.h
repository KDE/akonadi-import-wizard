/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#ifndef IMPORTFINISHPAGE_H
#define IMPORTFINISHPAGE_H

#include <QWidget>

namespace Ui {
class ImportFinishPage;
}

class ImportFinishPage : public QWidget
{
    Q_OBJECT
public:
    explicit ImportFinishPage(QWidget *parent = nullptr);
    ~ImportFinishPage();

    void addImportInfo(const QString &log);
    void addImportError(const QString &log);

private:
    Ui::ImportFinishPage *const ui;
};

#endif // IMPORTFINISHPAGE_H
