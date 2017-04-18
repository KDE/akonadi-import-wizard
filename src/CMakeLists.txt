

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

set(libimportwizard_abstract_SRCS
    abstract/abstractimporter.cpp
    abstract/abstractsettings.cpp
    abstract/abstractaddressbook.cpp
    abstract/abstractcalendar.cpp
    abstract/abstractbase.cpp
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


add_library(libimportwizard ${libimportwizard_autodetect_SRCS} ${libimportwizard_manual_SRCS} ${libimportwizard_SRCS} ${libimportwizard_abstract_SRCS})

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

install(TARGETS libimportwizard ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} ${LIBRARY_NAMELINK})

add_subdirectory(manual/pics)
add_subdirectory(plugins/)
