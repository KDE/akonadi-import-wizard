/*
   SPDX-FileCopyrightText: 2015-2021 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "selectprogramlistwidget.h"

#include <KLocalizedString>
#include <QPainter>

SelectProgramListWidget::SelectProgramListWidget(QWidget *parent)
    : QListWidget(parent)
{
}

SelectProgramListWidget::~SelectProgramListWidget()
{
}

void SelectProgramListWidget::setNoProgramFound(bool noProgramFound)
{
    mNoProgramFound = noProgramFound;
}

void SelectProgramListWidget::generalPaletteChanged()
{
    const QPalette palette = viewport()->palette();
    QColor color = palette.text().color();
    color.setAlpha(128);
    mTextColor = color;
}

void SelectProgramListWidget::paintEvent(QPaintEvent *event)
{
    if (mNoProgramFound && (!model() || model()->rowCount() == 0)) {
        QPainter p(viewport());

        QFont font = p.font();
        font.setItalic(true);
        p.setFont(font);

        if (!mTextColor.isValid()) {
            generalPaletteChanged();
        }
        p.setPen(mTextColor);

        p.drawText(QRect(0, 0, width(), height()), Qt::AlignCenter, i18n("No program found."));
    } else {
        QListWidget::paintEvent(event);
    }
}
