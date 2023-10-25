/*
   SPDX-FileCopyrightText: 2012-2023 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include "importwizard-version.h"
#include "importwizard.h"
#include <KAboutData>
#include <KCrash>
#include <KDBusService>
#include <KLocalizedString>
#include <QApplication>
#include <QIcon>

#include <QCommandLineParser>
#include <cstdio>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    KCrash::initialize();

    KLocalizedString::setApplicationDomain(QByteArrayLiteral("akonadiimportwizard"));
    // FIXME: "wizards" are "assistents" in new KDE slang

    KAboutData aboutData(QStringLiteral("importwizard"),
                         i18n("PIM Import Tool"),
                         QStringLiteral(KDEPIM_VERSION),
                         i18n("PIM Import Tool"),
                         KAboutLicense::GPL_V2,
                         i18n("Copyright © 2012-%1 ImportWizard authors", QStringLiteral("2023")));

    aboutData.addAuthor(i18n("Laurent Montel"), i18n("Maintainer"), QStringLiteral("montel@kde.org"));
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("kontact-import-wizard")));
    aboutData.setProductName(QByteArrayLiteral("kmail2/import wizard"));
    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    const QCommandLineOption modeOption(QStringList() << QStringLiteral("mode"), i18n("Mode: %1", QStringLiteral("manual|automatic")));
    parser.addOption(modeOption);
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    KDBusService service(KDBusService::Unique);

    ImportWizard::WizardMode mode = ImportWizard::WizardMode::AutoDetect;
    if (parser.isSet(modeOption)) {
        if (!parser.positionalArguments().isEmpty()) {
            const QString modeStr = parser.positionalArguments().at(0);
            if (modeStr == QLatin1String("manual")) {
                mode = ImportWizard::WizardMode::Manual;
            }
        }
    }

    auto wizard = new ImportWizard(mode);
    wizard->show();
    const int ret = app.exec();
    delete wizard;
    return ret;
}
