

configure_file(importwizard-version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/importwizard-version.h @ONLY)
add_subdirectory(icons)
add_subdirectory(kconf_update)
add_subdirectory(libimportwizard)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/icons/*-apps-kontact-import-wizard.png")
ecm_add_app_icon(importwizard_SRCS ICONS ${ICONS_SRCS})

set(importwizard_SRCS
    main.cpp
    importwizard.cpp
    importwizardkernel.cpp
    importmailpluginmanager.cpp
    importwizarddisplayinfo.cpp

    )

set(importwizard_manual_SRCS
    manual/manualimportmailpage.cpp
    manual/importwizardfilterinfogui.cpp
    manual/manualselectfilterpage.cpp
    )

set(importwizard_autodetect_SRCS
    autodetect/widgets/selectprogramlistwidget.cpp
    autodetect/selectprogrampage.cpp
    autodetect/importaddressbookpage.cpp
    autodetect/importfilterpage.cpp
    autodetect/importmailpage.cpp
    autodetect/importsettingpage.cpp
    autodetect/importfinishpage.cpp
    autodetect/selectcomponentpage.cpp
    autodetect/importfilterinfogui.cpp
    autodetect/importcalendarpage.cpp
    )


ki18n_wrap_ui(importwizard_SRCS
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


ecm_qt_declare_logging_category(importwizard_SRCS HEADER importwizard_debug.h IDENTIFIER IMPORTWIZARD_LOG CATEGORY_NAME org.kde.pim.importwizard)

add_executable(importwizard
    ${importwizard_SRCS}
    ${importwizard_manual_SRCS}
    ${importwizard_autodetect_SRCS}
    )

target_link_libraries(importwizard
    KPimImportWizard
    KF5::AkonadiCore
    KF5::XmlGui
    KF5::MailImporterAkonadi
    KF5::PimCommon
    KF5::MailCommon
    KF5::Libkdepim
    KF5::Completion
    KF5::I18n
    KF5::CoreAddons
    KF5::Crash
    KF5::DBusAddons
    Qt5::Widgets
    KF5::ConfigCore
    KF5::WidgetsAddons
    )

install(TARGETS importwizard  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS data/org.kde.importwizard.desktop DESTINATION ${KDE_INSTALL_APPDIR})

add_subdirectory(manual/pics)
add_subdirectory(plugins/)
