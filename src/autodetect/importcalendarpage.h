/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#ifndef IMPORTCALENDARPAGE_H
#define IMPORTCALENDARPAGE_H

#include <QWidget>

namespace Ui {
class ImportCalendarPage;
}

class ImportCalendarPage : public QWidget
{
    Q_OBJECT

public:
    explicit ImportCalendarPage(QWidget *parent = nullptr);
    ~ImportCalendarPage();

    void addImportInfo(const QString &log);
    void addImportError(const QString &log);
    void setImportButtonEnabled(bool enabled);

Q_SIGNALS:
    void importCalendarClicked();

private:
    Ui::ImportCalendarPage *const ui;
};

#endif // IMPORTCALENDARPAGE_H
