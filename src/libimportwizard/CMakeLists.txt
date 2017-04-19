set(libimportwizard_abstract_SRCS
    abstractimporter.cpp
    abstractsettings.cpp
    abstractaddressbook.cpp
    abstractcalendar.cpp
    abstractbase.cpp
    abstractdisplayinfo.cpp
    )
set(libimportwizard_utils_SRCS
    importwizardutil.cpp
    )

ecm_qt_declare_logging_category(libimportwizard_abstract_SRCS HEADER libimportwizard_debug.h IDENTIFIER LIBIMPORTWIZARD_LOG CATEGORY_NAME org.kde.pim.importwizard.lib)

add_library(libimportwizard ${libimportwizard_abstract_SRCS} ${libimportwizard_utils_SRCS})

generate_export_header(libimportwizard BASE_NAME libimportwizard)

target_link_libraries(libimportwizard
    PUBLIC
    KF5::MailTransport
    PRIVATE
    KF5::Archive
    KF5::PimCommon
    KF5::MailCommon
    KF5::I18n
    )

set_target_properties(libimportwizard
    PROPERTIES OUTPUT_NAME importwizard VERSION ${KDEPIM_LIB_VERSION} SOVERSION ${KDEPIM_LIB_SOVERSION}
    )


install(TARGETS libimportwizard ${KDE_INSTALL_TARGETS_DEFAULT_ARGS} ${LIBRARY_NAMELINK})

ecm_generate_headers(ImportWizard_CamelCase_HEADERS
    HEADER_NAMES
    AbstractBase
    AbstractDisplayInfo
    AbstractImporter
    AbstractSettings
    AbstractAddressBook
    AbstractCalendar
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
