cmake_minimum_required(VERSION 3.5)
set(PIM_VERSION "5.16.40")
project(AkonadiImportWizard VERSION ${PIM_VERSION})


set(KF5_MIN_VERSION "5.77.0")

find_package(ECM ${KF5_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH})
include(ECMInstallIcons)


include(ECMSetupVersion)
include(ECMAddTests)

include(GenerateExportHeader)
include(ECMGenerateHeaders)

include(FeatureSummary)
include(CheckFunctionExists)
include(ECMGeneratePriFile)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDEFrameworkCompilerSettings NO_POLICY_SCOPE)
include(ECMAddAppIcon)
include(ECMQtDeclareLoggingCategory)

# Do NOT add quote
set(KDEPIM_DEV_VERSION alpha)
set(RELEASE_SERVICE_VERSION "21.03.80")

# add an extra space
if(DEFINED KDEPIM_DEV_VERSION)
    set(KDEPIM_DEV_VERSION " ${KDEPIM_DEV_VERSION}")
endif()


set(PIM_VERSION "${PIM_VERSION}")
set(KDEPIM_VERSION "${PIM_VERSION}${KDEPIM_DEV_VERSION} (${RELEASE_SERVICE_VERSION})")


set(KMAILTRANSPORT_LIB_VERSION "5.16.40")
set(AKONADI_VERSION "5.16.40")

set(KDEPIM_LIB_VERSION "${PIM_VERSION}")
set(KDEPIM_LIB_SOVERSION "5")

set(QT_REQUIRED_VERSION "5.13.0")
find_package(Qt5 ${QT_REQUIRED_VERSION} CONFIG REQUIRED Widgets Gui)
set(MAILIMPORTER_VERSION "5.16.40")
set(MAILCOMMON_LIB_VERSION "5.16.40")
set(MESSAGELIB_LIB_VERSION "5.16.40")
set(PIMCOMMON_LIB_VERSION "5.16.40")
set(IDENTITYMANAGEMENT_LIB_VERSION "5.16.40")
set(GRANTLEETHEME_LIB_VERSION "5.16.40")

# Find KF5 package
find_package(KF5Config ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5DBusAddons ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5Auth ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5DocTools ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5Crash ${KF5_MIN_VERSION} REQUIRED)
find_package(KF5KIO ${KF5_MIN_VERSION} REQUIRED)

# Find KdepimLibs Package
find_package(KF5Akonadi ${AKONADI_VERSION} CONFIG REQUIRED)
find_package(KF5Contacts ${KF5_MIN_VERSION} CONFIG REQUIRED)
find_package(KF5IdentityManagement ${IDENTITYMANAGEMENT_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5MailTransport ${KMAILTRANSPORT_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5GrantleeTheme ${GRANTLEETHEME_LIB_VERSION} CONFIG REQUIRED)

find_package(KF5MailCommon ${MAILCOMMON_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5MailImporterAkonadi ${MAILIMPORTER_VERSION} CONFIG REQUIRED)
find_package(KF5MessageViewer ${MESSAGELIB_LIB_VERSION} CONFIG REQUIRED)
find_package(KF5PimCommonAkonadi ${PIMCOMMON_LIB_VERSION} CONFIG REQUIRED)
include_directories(${importwizard_SOURCE_DIR} ${importwizard_BINARY_DIR})

find_package(Qt5Keychain CONFIG)
set_package_properties(Qt5Keychain PROPERTIES
                                   DESCRIPTION "Provides support for secure credentials storage"
                                   URL "https://github.com/frankosterfeld/qtkeychain"
                                   TYPE REQUIRED)


add_definitions(-DQT_NO_URL_CAST_FROM_STRING)
add_definitions(-DQT_USE_QSTRINGBUILDER)
add_definitions(-DQT_DISABLE_DEPRECATED_BEFORE=0x050f00)
add_definitions(-DKF_DISABLE_DEPRECATED_BEFORE_AND_AT=0x054E00)

if(BUILD_TESTING)
   add_definitions(-DBUILD_TESTING)
endif()
add_definitions(-DQT_NO_FOREACH)
add_definitions(-DQT_NO_KEYWORDS)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(USE_UNITY_CMAKE_SUPPORT "Use UNITY cmake support (speedup compile time)" FALSE)

set(COMPILE_WITH_UNITY_CMAKE_SUPPORT false)
if (USE_UNITY_CMAKE_SUPPORT)
    if(${CMAKE_VERSION} VERSION_LESS "3.16.0")
        message(STATUS "CMAKE version is less than 3.16.0 . We can't use cmake unify build support")
    else()
        set(COMPILE_WITH_UNITY_CMAKE_SUPPORT true)
    endif()
endif()

add_subdirectory(src)

ecm_qt_install_logging_categories(
        EXPORT IMPORTWIZARD
        FILE importwizard.categories
        DESTINATION ${KDE_INSTALL_LOGGINGCATEGORIESDIR}
        )

add_subdirectory(doc)
feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
