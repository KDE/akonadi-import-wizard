ecm_setup_version(PROJECT
    VARIABLE_PREFIX IMPORTWIZARD
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/importwizard_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPim6ImportWizardConfigVersion.cmake"
    SOVERSION 6
)

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPim6ImportWizard")
set(IMPORTWIZARD_KF6_COMPAT FALSE)

add_library(KPim6ImportWizard)
add_library(KPim6::ImportWizard ALIAS KPim6ImportWizard)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimImportWizardConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6ImportWizardConfig.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6ImportWizardConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6ImportWizardConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
)

install(EXPORT KPim6ImportWizardTargets
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    FILE KPim6ImportWizardTargets.cmake
    NAMESPACE KPim6::
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/importwizard_version.h"
    DESTINATION "${KDE_INSTALL_INCLUDEDIR}/KPim6/ImportWizard/"
    COMPONENT Devel
)



target_sources(KPim6ImportWizard PRIVATE
    abstractimporter.cpp
    abstractsettings.cpp
    abstractaddressbook.cpp
    abstractcalendar.cpp
    abstractbase.cpp
    abstractdisplayinfo.cpp
    importwizardutil.cpp
    importwizardsavepasswordjob.cpp
    abstractsettings.h
    abstractbase.h
    importwizardutil.h
    abstractaddressbook.h
    importwizardsavepasswordjob.h
    abstractimporter.h
    abstractdisplayinfo.h
    abstractcalendar.h
    )

ecm_qt_declare_logging_category(KPim6ImportWizard HEADER libimportwizard_debug.h IDENTIFIER LIBIMPORTWIZARD_LOG CATEGORY_NAME org.kde.pim.importwizard.lib
        DESCRIPTION "importwizard (libimportwizard)"
        EXPORT IMPORTWIZARD
    )

if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KPim6ImportWizard PROPERTIES UNITY_BUILD ON)
endif()


target_include_directories(KPim6ImportWizard INTERFACE
    "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KPim6/ImportWizard;${KDE_INSTALL_INCLUDEDIR}/KPim6/>")


generate_export_header(KPim6ImportWizard BASE_NAME libimportwizard)

target_link_libraries(KPim6ImportWizard
    PUBLIC
    KPim6::MailTransport
    PRIVATE
    KF6::Archive
    KPim6::PimCommon
    KPim6::MailCommon
    KPim6::MailImporter
    KPim6::MailImporterAkonadi
    KF6::I18n
    )
target_link_libraries(KPim6ImportWizard PRIVATE qt${QT_MAJOR_VERSION}keychain)
set_target_properties(KPim6ImportWizard PROPERTIES
    VERSION ${IMPORTWIZARD_VERSION}
    SOVERSION ${IMPORTWIZARD_SOVERSION}
    EXPORT_NAME ImportWizard
)

install(TARGETS
    KPim6ImportWizard
    EXPORT KPim6ImportWizardTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
)


install(TARGETS KPim6ImportWizard ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

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
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/ImportWizard/ImportWizard/
    COMPONENT Devel
    )

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/libimportwizard_export.h
    ${ImportWizard_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim6/ImportWizard/importwizard/
    COMPONENT Devel
    )

if (QT_MAJOR_VERSION STREQUAL "5")
##
# TODO: Backwards compatiblity. Remove in next major version
##
set(CMAKECONFIG_INSTALL_DIR_KF6 "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPimImportWizard")
set(IMPORTWIZARD_KF6_COMPAT TRUE)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimImportWizardConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPimImportWizardConfig.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR_KF6}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPimImportWizardConfig.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR_KF6}"
    COMPONENT Devel
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPim6ImportWizardConfigVersion.cmake"
    RENAME "KPimImportWizardConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR_KF6}"
    COMPONENT Devel
)
install(EXPORT KPim6ImportWizardTargets
    DESTINATION "${CMAKECONFIG_INSTALL_DIR_KF6}"
    FILE KPim6ImportWizardTargets.cmake
    NAMESPACE KPim::
)
endif()
