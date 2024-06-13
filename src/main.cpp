/*
   SPDX-FileCopyrightText: 2012-2024 Laurent Montel <montel@kde.org>

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
#define HAVE_KICONTHEME __has_include(<KIconTheme>)
#if HAVE_KICONTHEME
#include <KIconTheme>
#endif

#define HAVE_STYLE_MANAGER __has_include(<KStyleManager>)
#if HAVE_STYLE_MANAGER
#include <KStyleManager>
#endif

int main(int argc, char *argv[])
{
#if HAVE_KICONTHEME
    KIconTheme::initTheme();
#endif
    QApplication app(argc, argv);
    KCrash::initialize();
#if HAVE_STYLE_MANAGER
    KStyleManager::initStyle();
#else // !HAVE_STYLE_MANAGER
#if defined(Q_OS_MACOS) || defined(Q_OS_WIN)
    QApplication::setStyle(QStringLiteral("breeze"));
#endif // defined(Q_OS_MACOS) || defined(Q_OS_WIN)
#endif // HAVE_STYLE_MANAGER

    KLocalizedString::setApplicationDomain(QByteArrayLiteral("akonadiimportwizard"));
    // FIXME: "wizards" are "assistents" in new KDE slang

    KAboutData aboutData(QStringLiteral("importwizard"),
                         i18n("PIM Import Tool"),
                         QStringLiteral(KDEPIM_VERSION),
                         i18n("PIM Import Tool"),
                         KAboutLicense::GPL_V2,
                         i18n("Copyright © 2012-%1 ImportWizard authors", QStringLiteral("2023")));

    aboutData.addAuthor(i18nc("@info:credit", "Laurent Montel"), i18n("Maintainer"), QStringLiteral("montel@kde.org"));
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("kontact-import-wizard")));
    aboutData.setProductName(QByteArrayLiteral("kmail2/import wizard"));
    KAboutData::setApplicationData(aboutData);

    QCommandLineParser parser;
    const QCommandLineOption modeOption(QStringList() << QStringLiteral("mode"), i18nc("@info:shell", "Mode: %1", QStringLiteral("manual|automatic")));
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
