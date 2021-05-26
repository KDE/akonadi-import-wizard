ecm_setup_version(PROJECT
    VARIABLE_PREFIX IMPORTWIZARD
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/importwizard_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPimImportWizardConfigVersion.cmake"
    SOVERSION 5
)

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPimImportWizard")

add_library(KPimImportWizard)
add_library(KPim::ImportWizard ALIAS KPimImportWizard)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimImportWizardConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPimImportWizardConfig.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPimImportWizardConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KPimImportWizardConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
)

install(EXPORT KPimImportWizardTargets
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    FILE KPimImportWizardTargets.cmake
    NAMESPACE KPim::
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/importwizard_version.h"
    DESTINATION "${KDE_INSTALL_INCLUDEDIR}/KPim/"
    COMPONENT Devel
)



target_sources(KPimImportWizard PRIVATE
    abstractimporter.cpp
    abstractsettings.cpp
    abstractaddressbook.cpp
    abstractcalendar.cpp
    abstractbase.cpp
    abstractdisplayinfo.cpp
    importwizardutil.cpp
    importwizardsavepasswordjob.cpp
    )

ecm_qt_declare_logging_category(KPimImportWizard HEADER libimportwizard_debug.h IDENTIFIER LIBIMPORTWIZARD_LOG CATEGORY_NAME org.kde.pim.importwizard.lib
        DESCRIPTION "importwizard (libimportwizard)"
        EXPORT IMPORTWIZARD
    )

if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KPimImportWizard PROPERTIES UNITY_BUILD ON)
endif()
add_library(KPim::ImportWizard ALIAS KPimImportWizard)


target_include_directories(KPimImportWizard INTERFACE "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR_KF5}/KPim/ImportWizard;${KDE_INSTALL_INCLUDEDIR_KF5}/KPim/>")


generate_export_header(KPimImportWizard BASE_NAME libimportwizard)

target_link_libraries(KPimImportWizard
    PUBLIC
    KF5::MailTransport
    PRIVATE
    KF5::Archive
    KF5::PimCommon
    KF5::MailCommon
    KF5::MailImporter
    KF5::I18n
    qt5keychain
    )

set_target_properties(KPimImportWizard PROPERTIES
    VERSION ${IMPORTWIZARD_VERSION}
    SOVERSION ${IMPORTWIZARD_SOVERSION}
    EXPORT_NAME ImportWizard
)

install(TARGETS
    KPimImportWizard
    EXPORT KPimImportWizardTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
)


install(TARGETS KPimImportWizard ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

ecm_generate_headers(ImportWizard_CamelCase_HEADERS
    HEADER_NAMES
    AbstractBase
    AbstractDisplayInfo
    AbstractImporter
    AbstractSettings
    AbstractAddressBook
    AbstractCalendar
    ImportWizardUtil
    PREFIX ImportWizard
    REQUIRED_HEADERS ImportWizard_HEADERS
    )

install(FILES
    ${ImportWizard_CamelCase_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/KPim/ImportWizard/
    COMPONENT Devel
    )

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/libimportwizard_export.h
    ${ImportWizard_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR_KF5}/KPim/importwizard/
    COMPONENT Devel
    )

