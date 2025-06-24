/*
   SPDX-FileCopyrightText: 2012-2025 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/
#include "evolutionaddressbook.h"
using namespace Qt::Literals::StringLiterals;

#include "abstractdisplayinfo.h"

#include <KLocalizedString>
#include <KMessageBox>
#include <QFileDialog>
#include <QProcess>

EvolutionAddressBook::EvolutionAddressBook() = default;

EvolutionAddressBook::~EvolutionAddressBook() = default;

void EvolutionAddressBook::exportEvolutionAddressBook()
{
    KMessageBox::information(mAbstractDisplayInfo->parentWidget(),
                             i18n("Evolution address book will be exported as vCard. Import vCard in KAddressBook."),
                             i18nc("@title:window", "Export Evolution Address Book"));

    const QString directory =
        QFileDialog::getExistingDirectory(mAbstractDisplayInfo->parentWidget(), i18nc("@title:window", "Select the directory where vCards will be stored."));
    if (directory.isEmpty()) {
        return;
    }
    QFile evolutionFile;
    bool found = false;
    for (int i = 0; i < 9; ++i) {
        evolutionFile.setFileName(u"/usr/lib/evolution/3.%1/evolution-addressbook-export"_s.arg(i));
        if (evolutionFile.exists()) {
            found = true;
            break;
        }
    }
    if (found) {
        QStringList arguments;
        arguments << u"-l"_s;
        QProcess proc;
        proc.start(evolutionFile.fileName(), arguments);
        if (!proc.waitForFinished()) {
            return;
        }
        QByteArray result = proc.readAll();
        proc.close();
        if (!result.isEmpty()) {
            result.replace('\n', ',');
            const QString value(QString::fromLatin1(result.trimmed()));
            const QStringList listAddressBook = value.split(u',');
            // qCDebug(EVOLUTIONPLUGIN_LOG)<<" listAddressBook"<<listAddressBook;
            int i = 0;
            QString name;
            QString displayname;
            for (const QString &arg : listAddressBook) {
                switch (i) {
                case 0:
                    name = arg;
                    name.remove(0, 1);
                    name.remove(name.length() - 1, 1);
                    ++i;
                    // name
                    break;
                case 1:
                    displayname = arg;
                    displayname.remove(0, 1);
                    displayname.remove(displayname.length() - 1, 1);
                    // display name
                    ++i;
                    break;
                case 2:
                    if (!displayname.isEmpty() && !name.isEmpty()) {
                        arguments.clear();
                        arguments << u"--format=vcard"_s << name << u"--output=%1/%2.vcard"_s.arg(directory, displayname);
                        proc.start(evolutionFile.fileName(), arguments);
                        if (proc.waitForFinished()) {
                            addAddressBookImportInfo(i18n("Address book \"%1\" exported.", displayname));
                        } else {
                            addAddressBookImportError(i18n("Failed to export address book \"%1\".", displayname));
                        }
                    }
                    i = 0; // reset
                    break;
                }
            }
        }
    }
}

#include "moc_evolutionaddressbook.cpp"
