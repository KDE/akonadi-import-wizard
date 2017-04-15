

configure_file(importwizard-version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/importwizard-version.h @ONLY)
add_subdirectory(icons)
add_subdirectory(kconf_update)

set(libimportwizard_SRCS
    importwizard.cpp
    importwizardkernel.cpp
    importmailpluginmanager.cpp
    )

set(libimportwizard_manual_SRCS
    manual/manualimportmailpage.cpp
    manual/importwizardfilterinfogui.cpp
    manual/manualselectfilterpage.cpp
    )

set(libimportwizard_autodetect_SRCS
    autodetect/widgets/selectprogramlistwidget.cpp
    autodetect/importwizardutil.cpp
    autodetect/selectprogrampage.cpp
    autodetect/importaddressbookpage.cpp
    autodetect/importfilterpage.cpp
    autodetect/importmailpage.cpp
    autodetect/importsettingpage.cpp
    autodetect/importfinishpage.cpp
    autodetect/selectcomponentpage.cpp
    autodetect/importfilterinfogui.cpp
    autodetect/importcalendarpage.cpp
    autodetect/abstractimporter.cpp
    autodetect/abstractsettings.cpp
    autodetect/abstractaddressbook.cpp
    autodetect/abstractcalendar.cpp
    autodetect/abstractbase.cpp
    autodetect/thunderbird/thunderbirdimportdata.cpp
    autodetect/thunderbird/thunderbirdsettings.cpp
    autodetect/thunderbird/thunderbirdaddressbook.cpp
    autodetect/thunderbird/addressbook/MorkParser.cpp
    autodetect/sylpheed/sylpheedimportdata.cpp
    autodetect/sylpheed/sylpheedsettings.cpp
    autodetect/sylpheed/sylpheedaddressbook.cpp
    autodetect/sylpheed/sylpheedsettingsutils.cpp
    autodetect/evolutionv3/evolutionv3importdata.cpp
    autodetect/evolutionv3/evolutionsettings.cpp
    autodetect/evolutionv3/evolutioncalendar.cpp
    autodetect/evolutionv3/evolutionaddressbook.cpp
    autodetect/evolutionv3/evolutionutil.cpp
    autodetect/evolutionv2/evolutionv2importdata.cpp
    autodetect/evolutionv1/evolutionv1importdata.cpp
    autodetect/opera/operaimportdata.cpp
    autodetect/opera/operaaddressbook.cpp
    autodetect/opera/operasettings.cpp
    autodetect/oe/oeimportdata.cpp
    autodetect/mailapp/mailappimportdata.cpp
    autodetect/pmail/pmailimportdata.cpp
    autodetect/pmail/pmailsettings.cpp
    autodetect/thebat/thebatimportdata.cpp
    autodetect/balsa/balsaimportdata.cpp
    autodetect/balsa/balsasettings.cpp
    autodetect/balsa/balsaaddressbook.cpp
    autodetect/claws-mail/clawsmailimportdata.cpp
    autodetect/claws-mail/clawsmailsettings.cpp
    autodetect/claws-mail/clawsmailaddressbook.cpp
    autodetect/trojita/trojitasettings.cpp
    autodetect/trojita/trojitaimportdata.cpp
    autodetect/trojita/trojitaaddressbook.cpp
    autodetect/icedove/icedoveimportdata.cpp
    autodetect/geary/gearysettings.cpp
    autodetect/geary/gearyimportdata.cpp
    autodetect/geary/gearyaddressbook.cpp
    )


ki18n_wrap_ui(libimportwizard_SRCS
    ui/autodetect/selectprogrampage.ui
    ui/autodetect/importaddressbookpage.ui
    ui/autodetect/importfilterpage.ui
    ui/autodetect/importmailpage.ui
    ui/autodetect/importsettingpage.ui
    ui/autodetect/selectcomponentpage.ui
    ui/autodetect/importfinishpage.ui
    ui/autodetect/importcalendarpage.ui
    ui/manual/manualimportmailpage.ui
    ui/manual/manualselectfilterpage.ui
    )

ecm_qt_declare_logging_category(libimportwizard_SRCS HEADER importwizard_debug.h IDENTIFIER IMPORTWIZARD_LOG CATEGORY_NAME org.kde.pim.importwizard)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/icons/*-apps-kontact-import-wizard.png")
ecm_add_app_icon(importwizard_SRCS ICONS ${ICONS_SRCS})


add_library(libimportwizard ${libimportwizard_autodetect_SRCS} ${libimportwizard_manual_SRCS} ${libimportwizard_SRCS})

generate_export_header(libimportwizard BASE_NAME libimportwizard)

target_link_libraries(libimportwizard
    PRIVATE
    KF5::AkonadiCore
    KF5::MailTransport
    KF5::XmlGui
    KF5::MailImporterAkonadi
    KF5::PimCommon
    KF5::MailCommon
    KF5::Libkdepim
    KF5::MessageViewer
    KF5::Completion
    KF5::I18n
    KF5::CoreAddons
    )

set_target_properties(libimportwizard
    PROPERTIES OUTPUT_NAME importwizard VERSION ${KDEPIM_LIB_VERSION} SOVERSION ${KDEPIM_LIB_SOVERSION}
    )


set(importwizard_SRCS
    main.cpp
    )

add_executable(importwizard ${importwizard_SRCS})

target_link_libraries(importwizard
    libimportwizard
    KF5::I18n
    KF5::Crash
    KF5::CoreAddons
    KF5::DBusAddons
    Qt5::Widgets
    KF5::ConfigCore
    KF5::WidgetsAddons
    KF5::MailCommon
    )

install(TARGETS importwizard  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS data/org.kde.importwizard.desktop DESTINATION ${KDE_INSTALL_APPDIR})

install(TARGETS libimportwizard ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} LIBRARY NAMELINK_SKIP)

add_subdirectory(manual/pics)
add_subdirectory(autodetect/)
