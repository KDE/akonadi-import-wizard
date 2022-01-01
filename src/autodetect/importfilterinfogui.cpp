/*
   SPDX-FileCopyrightText: 2012-2022 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "importfilterinfogui.h"
#include "importmailpage.h"

#include <MailImporter/ImportMailsWidget>

#include <KMessageBox>
#include <QApplication>

#include <QListWidgetItem>

ImportFilterInfoGui::ImportFilterInfoGui(ImportMailPage *parent)
    : MailImporter::FilterInfoGui()
    , m_parent(parent)
{
}

ImportFilterInfoGui::~ImportFilterInfoGui() = default;

void ImportFilterInfoGui::setStatusMessage(const QString &status)
{
    m_parent->mailWidget()->setStatusMessage(status);
}

void ImportFilterInfoGui::setFrom(const QString &from)
{
    m_parent->mailWidget()->setFrom(from);
}

void ImportFilterInfoGui::setTo(const QString &to)
{
    m_parent->mailWidget()->setTo(to);
}

void ImportFilterInfoGui::setCurrent(const QString &current)
{
    m_parent->mailWidget()->setCurrent(current);
    qApp->processEvents();
}

void ImportFilterInfoGui::setCurrent(int percent)
{
    m_parent->mailWidget()->setCurrent(percent);
    qApp->processEvents(); // Be careful - back & finish buttons disabled, so only user event that can happen is cancel/close button
}

void ImportFilterInfoGui::setOverall(int percent)
{
    m_parent->mailWidget()->setOverall(percent);
}

void ImportFilterInfoGui::addInfoLogEntry(const QString &log)
{
    auto item = new QListWidgetItem(log);
    item->setForeground(Qt::blue);
    m_parent->mailWidget()->addItem(item);
    m_parent->mailWidget()->setLastCurrentItem();
    qApp->processEvents();
}

void ImportFilterInfoGui::addErrorLogEntry(const QString &log)
{
    auto item = new QListWidgetItem(log);
    item->setForeground(Qt::red);
    m_parent->mailWidget()->addItem(item);
    m_parent->mailWidget()->setLastCurrentItem();
    qApp->processEvents();
}

void ImportFilterInfoGui::clear()
{
    m_parent->mailWidget()->clear();
}

void ImportFilterInfoGui::alert(const QString &message)
{
    KMessageBox::information(m_parent, message);
}

QWidget *ImportFilterInfoGui::parent() const
{
    return m_parent;
}
