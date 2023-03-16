ecm_setup_version(PROJECT
    VARIABLE_PREFIX IMPORTWIZARD
    VERSION_HEADER "${CMAKE_CURRENT_BINARY_DIR}/importwizard_version.h"
    PACKAGE_VERSION_FILE "${CMAKE_CURRENT_BINARY_DIR}/KPim${KF_MAJOR_VERSION}ImportWizardConfigVersion.cmake"
    SOVERSION 5
)

set(CMAKECONFIG_INSTALL_DIR "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPim${KF_MAJOR_VERSION}ImportWizard")
set(IMPORTWIZARD_KF5_COMPAT FALSE)

add_library(KPim${KF_MAJOR_VERSION}ImportWizard)
add_library(KPim${KF_MAJOR_VERSION}::ImportWizard ALIAS KPim${KF_MAJOR_VERSION}ImportWizard)

configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimImportWizardConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim${KF_MAJOR_VERSION}ImportWizardConfig.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPim${KF_MAJOR_VERSION}ImportWizardConfig.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/KPim${KF_MAJOR_VERSION}ImportWizardConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    COMPONENT Devel
)

install(EXPORT KPim${KF_MAJOR_VERSION}ImportWizardTargets
    DESTINATION "${CMAKECONFIG_INSTALL_DIR}"
    FILE KPim${KF_MAJOR_VERSION}ImportWizardTargets.cmake
    NAMESPACE KPim${KF_MAJOR_VERSION}::
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/importwizard_version.h"
    DESTINATION "${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/"
    COMPONENT Devel
)



target_sources(KPim${KF_MAJOR_VERSION}ImportWizard PRIVATE
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

ecm_qt_declare_logging_category(KPim${KF_MAJOR_VERSION}ImportWizard HEADER libimportwizard_debug.h IDENTIFIER LIBIMPORTWIZARD_LOG CATEGORY_NAME org.kde.pim.importwizard.lib
        DESCRIPTION "importwizard (libimportwizard)"
        EXPORT IMPORTWIZARD
    )

if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(KPim${KF_MAJOR_VERSION}ImportWizard PROPERTIES UNITY_BUILD ON)
endif()


target_include_directories(KPim${KF_MAJOR_VERSION}ImportWizard INTERFACE
    "$<INSTALL_INTERFACE:${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/ImportWizard;${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/>")


generate_export_header(KPim${KF_MAJOR_VERSION}ImportWizard BASE_NAME libimportwizard)

target_link_libraries(KPim${KF_MAJOR_VERSION}ImportWizard
    PUBLIC
    KPim${KF_MAJOR_VERSION}::MailTransport
    PRIVATE
    KF${KF_MAJOR_VERSION}::Archive
    KPim${KF_MAJOR_VERSION}::PimCommon
    KPim${KF_MAJOR_VERSION}::MailCommon
    KPim${KF_MAJOR_VERSION}::MailImporter
    KPim${KF_MAJOR_VERSION}::MailImporterAkonadi
    KF${KF_MAJOR_VERSION}::I18n
    )
target_link_libraries(KPim${KF_MAJOR_VERSION}ImportWizard PRIVATE qt${QT_MAJOR_VERSION}keychain)
set_target_properties(KPim${KF_MAJOR_VERSION}ImportWizard PROPERTIES
    VERSION ${IMPORTWIZARD_VERSION}
    SOVERSION ${IMPORTWIZARD_SOVERSION}
    EXPORT_NAME ImportWizard
)

install(TARGETS
    KPim${KF_MAJOR_VERSION}ImportWizard
    EXPORT KPim${KF_MAJOR_VERSION}ImportWizardTargets ${KDE_INSTALL_TARGETS_DEFAULT_ARGS}
)


install(TARGETS KPim${KF_MAJOR_VERSION}ImportWizard ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

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
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/ImportWizard/
    COMPONENT Devel
    )

install(FILES
    ${CMAKE_CURRENT_BINARY_DIR}/libimportwizard_export.h
    ${ImportWizard_HEADERS}
    DESTINATION ${KDE_INSTALL_INCLUDEDIR}/KPim${KF_MAJOR_VERSION}/importwizard/
    COMPONENT Devel
    )

if (QT_MAJOR_VERSION STREQUAL "5")
##
# TODO: Backwards compatiblity. Remove in next major version
##
set(CMAKECONFIG_INSTALL_DIR_KF5 "${KDE_INSTALL_CMAKEPACKAGEDIR}/KPimImportWizard")
set(IMPORTWIZARD_KF5_COMPAT TRUE)
configure_package_config_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/KPimImportWizardConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/KPimImportWizardConfig.cmake"
    INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR_KF5}
)

install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPimImportWizardConfig.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR_KF5}"
    COMPONENT Devel
)
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/KPim${KF_MAJOR_VERSION}ImportWizardConfigVersion.cmake"
    RENAME "KPimImportWizardConfigVersion.cmake"
    DESTINATION "${CMAKECONFIG_INSTALL_DIR_KF5}"
    COMPONENT Devel
)
install(EXPORT KPim${KF_MAJOR_VERSION}ImportWizardTargets
    DESTINATION "${CMAKECONFIG_INSTALL_DIR_KF5}"
    FILE KPim${KF_MAJOR_VERSION}ImportWizardTargets.cmake
    NAMESPACE KPim::
)
endif()
