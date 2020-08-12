/*
   SPDX-FileCopyrightText: 2012-2020 Laurent Montel <montel@kde.org>

   SPDX-License-Identifier: GPL-2.0-or-later
*/

#include <KAboutData>
#include <KDBusService>
#include <KLocalizedString>
#include <Kdelibs4ConfigMigrator>
#include <KCrash>
#include <QApplication>
#include <QIcon>
#include "importwizard.h"

#include "importwizard-version.h"

#include <QCommandLineParser>
#include <stdio.h>

int main(int argc, char *argv[])
{
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    app.setAttribute(Qt::AA_UseHighDpiPixmaps, true);
    KCrash::initialize();
    Kdelibs4ConfigMigrator migrate(QStringLiteral("importwizard"));
    migrate.setConfigFiles(QStringList() << QStringLiteral("importwizardrc"));
    migrate.migrate();

    KLocalizedString::setApplicationDomain("akonadiimportwizard");
    //FIXME: "wizards" are "assistents" in new KDE slang

    KAboutData aboutData(QStringLiteral("importwizard"),
                         i18n("PIM Import Tool"),
                         QStringLiteral(KDEPIM_VERSION),
                         i18n("PIM Import Tool"),
                         KAboutLicense::GPL_V2,
                         i18n("Copyright © 2012-2020 ImportWizard authors"));

    aboutData.addAuthor(i18n("Laurent Montel"), i18n("Maintainer"), QStringLiteral("montel@kde.org"));
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("kontact-import-wizard")));
    aboutData.setOrganizationDomain(QByteArrayLiteral("kde.org"));
    aboutData.setProductName(QByteArrayLiteral("importwizard"));
    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    parser.addOption(QCommandLineOption(QStringList() <<  QStringLiteral("mode"), i18n("Mode: %1", QStringLiteral("manual|automatic"))));
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    KDBusService service(KDBusService::Unique);

    ImportWizard::WizardMode mode = ImportWizard::WizardMode::AutoDetect;
    if (parser.isSet(QStringLiteral("mode"))) {
        if (!parser.positionalArguments().isEmpty()) {
            const QString modeStr = parser.positionalArguments().at(0);
            if (modeStr == QLatin1String("manual")) {
                mode = ImportWizard::WizardMode::Manual;
            }
        }
    }

    ImportWizard *wizard = new ImportWizard(mode);
    wizard->show();
    const int ret = app.exec();
    delete wizard;
    return ret;
}
