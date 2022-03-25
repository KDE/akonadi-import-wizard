/*
   SPDX-FileCopyrightText: 2012-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#pragma once

#include <QWidget>

class QListWidgetItem;

namespace Ui
{
class SelectProgramPage;
}

class SelectProgramPage : public QWidget
{
    Q_OBJECT

public:
    explicit SelectProgramPage(QWidget *parent = nullptr);
    ~SelectProgramPage() override;

    void setFoundProgram(const QStringList &list);
    void disableSelectProgram();

Q_SIGNALS:
    void programSelected(const QString &);
    void doubleClicked();
    void selectManualSelectionChanged(bool);

private:
    void slotItemSelectionChanged();
    void slotItemDoubleClicked(QListWidgetItem *item);

    void slotSelectManualSelectionChanged(bool b);
    Ui::SelectProgramPage *const ui;
};
