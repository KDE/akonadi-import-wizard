set(icedoveimporter_SRCS
    icedoveimportdata.cpp
    ../thunderbird/thunderbirdaddressbook.cpp
    ../thunderbird/thunderbirdsettings.cpp
    ../thunderbird/addressbook/MorkParser.cpp
    icedoveimportdata.h
    )

ecm_qt_declare_logging_category(icedoveimporter_SRCS HEADER thunderbirdplugin_debug.h
        IDENTIFIER THUNDERBIRDPLUGIN_LOG CATEGORY_NAME org.kde.pim.importwizard.thunderbirdplugin
        DESCRIPTION "importwizard (thunderbird plugin)"
        EXPORT IMPORTWIZARD
    )

kcoreaddons_add_plugin(icedoveimporterplugin SOURCES ${icedoveimporter_SRCS} INSTALL_NAMESPACE pim${QT_MAJOR_VERSION}/importwizard)
set_target_properties(icedoveimporterplugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/importwizard/")
if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(icedoveimporterplugin PROPERTIES UNITY_BUILD ON)
endif()

target_link_libraries(icedoveimporterplugin
    KPim${KF_MAJOR_VERSION}::ImportWizard
    KF${KF_MAJOR_VERSION}::XmlGui
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::Contacts
    KPim${KF_MAJOR_VERSION}::MailImporterAkonadi
    KPim${KF_MAJOR_VERSION}::MailTransport
    KPim${KF_MAJOR_VERSION}::IdentityManagement
    KPim${KF_MAJOR_VERSION}::MailCommon
    )
