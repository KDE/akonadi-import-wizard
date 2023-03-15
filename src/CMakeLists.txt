

configure_file(importwizard-version.h.cmake ${CMAKE_CURRENT_BINARY_DIR}/importwizard-version.h @ONLY)
add_subdirectory(icons)
add_subdirectory(libimportwizard)

file(GLOB ICONS_SRCS "${CMAKE_CURRENT_SOURCE_DIR}/icons/*-apps-kontact-import-wizard.png")
ecm_add_app_icon(importwizard_SRCS ICONS ${ICONS_SRCS})
add_executable(akonadiimportwizard)

target_sources(akonadiimportwizard PRIVATE
    main.cpp
    importwizard.cpp
    importwizardkernel.cpp
    importmailpluginmanager.cpp
    importwizarddisplayinfo.cpp
    manual/manualimportmailpage.cpp
    manual/importwizardfilterinfogui.cpp
    manual/manualselectfilterpage.cpp
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

    importmailpluginmanager.h
    autodetect/importaddressbookpage.h
    autodetect/selectprogrampage.h
    autodetect/importfilterpage.h
    autodetect/selectcomponentpage.h
    autodetect/importfinishpage.h
    autodetect/importmailpage.h
    autodetect/widgets/selectprogramlistwidget.h
    autodetect/importsettingpage.h
    autodetect/importfilterinfogui.h
    autodetect/importcalendarpage.h
    importwizardkernel.h
    importwizarddisplayinfo.h
    libimportwizard_private_export.h
    importwizard.h
    manual/manualselectfilterpage.h
    manual/manualimportmailpage.h
    manual/importwizardfilterinfogui.h
    )


ki18n_wrap_ui(akonadiimportwizard
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

ecm_qt_declare_logging_category(akonadiimportwizard HEADER importwizard_debug.h IDENTIFIER IMPORTWIZARD_LOG CATEGORY_NAME org.kde.pim.importwizard
        DESCRIPTION "importwizard (importwizard)"
        OLD_CATEGORY_NAMES log_importwizard
        EXPORT IMPORTWIZARD
    )


if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(akonadiimportwizard PROPERTIES UNITY_BUILD ON)
endif()
target_link_libraries(akonadiimportwizard
    KPim${KF_MAJOR_VERSION}ImportWizard
    KPim${KF_MAJOR_VERSION}::AkonadiCore
    KF${KF_MAJOR_VERSION}::XmlGui
    KPim${KF_MAJOR_VERSION}::MailImporterAkonadi
    KPim${KF_MAJOR_VERSION}::PimCommon
    KPim${KF_MAJOR_VERSION}::MailCommon
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::CoreAddons
    KF${KF_MAJOR_VERSION}::Crash
    KF${KF_MAJOR_VERSION}::DBusAddons
    Qt::Widgets
    KF${KF_MAJOR_VERSION}::ConfigCore
    KF${KF_MAJOR_VERSION}::WidgetsAddons
    )

install(TARGETS akonadiimportwizard  ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})
install(PROGRAMS data/org.kde.akonadiimportwizard.desktop DESTINATION ${KDE_INSTALL_APPDIR})

add_subdirectory(manual/pics)
add_subdirectory(plugins/)
