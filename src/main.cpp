/*
   SPDX-FileCopyrightText: 2012-2026 Laurent Montel <montel@kde.org>

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

#include <KIconTheme>
#include <QCommandLineParser>
#include <cstdio>

#include <KStyleManager>

using namespace Qt::Literals::StringLiterals;
int main(int argc, char *argv[])
{
    KIconTheme::initTheme();
    QApplication app(argc, argv);
    KStyleManager::initStyle();

    KLocalizedString::setApplicationDomain(QByteArrayLiteral("akonadiimportwizard"));
    // FIXME: "wizards" are "assistants" in new KDE slang

    KAboutData aboutData(u"importwizard"_s,
                         i18n("PIM Import Tool"),
                         QStringLiteral(KDEPIM_VERSION),
                         i18n("PIM Import Tool"),
                         KAboutLicense::GPL_V2,
                         i18n("Copyright © 2012-%1 ImportWizard authors", u"2026"_s));

    aboutData.addAuthor(i18nc("@info:credit", "Laurent Montel"), i18n("Maintainer"), u"montel@kde.org"_s);
    QApplication::setWindowIcon(QIcon::fromTheme(u"kontact-import-wizard"_s));
    aboutData.setProductName(QByteArrayLiteral("kmail2/import wizard"));
    aboutData.setDesktopFileName(u"org.kde.akonadiimportwizard"_s);
    KAboutData::setApplicationData(aboutData);
    KCrash::initialize();

    QCommandLineParser parser;
    const QCommandLineOption modeOption(QStringList() << u"mode"_s, i18nc("@info:shell", "Mode: %1", u"manual|automatic"_s));
    parser.addOption(modeOption);
    aboutData.setupCommandLine(&parser);
    parser.process(app);
    aboutData.processCommandLine(&parser);

    KDBusService service(KDBusService::Unique);

    ImportWizard::WizardMode mode = ImportWizard::WizardMode::AutoDetect;
    if (parser.isSet(modeOption)) {
        if (!parser.positionalArguments().isEmpty()) {
            const QString modeStr = parser.positionalArguments().at(0);
            if (modeStr == QLatin1StringView("manual")) {
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
