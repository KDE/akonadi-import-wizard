set(seamonkeyimporter_SRCS
    seamonkeyimportdata.cpp
    ../thunderbird/thunderbirdaddressbook.cpp
    ../thunderbird/thunderbirdsettings.cpp
    ../thunderbird/addressbook/MorkParser.cpp
    seamonkeyimportdata.h
    )

ecm_qt_declare_logging_category(seamonkeyimporter_SRCS HEADER thunderbirdplugin_debug.h
        IDENTIFIER THUNDERBIRDPLUGIN_LOG CATEGORY_NAME org.kde.pim.importwizard.thunderbirdplugin
        DESCRIPTION "importwizard (thunderbird plugin)"
        EXPORT IMPORTWIZARD
    )

kcoreaddons_add_plugin(seamonkeyimporterplugin SOURCES ${seamonkeyimporter_SRCS} INSTALL_NAMESPACE pim${QT_MAJOR_VERSION}/importwizard)
set_target_properties(seamonkeyimporterplugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/importwizard/")
if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(seamonkeyimporterplugin PROPERTIES UNITY_BUILD ON)
endif()

target_link_libraries(seamonkeyimporterplugin
    KPim::ImportWizard
    KF${KF_MAJOR_VERSION}::XmlGui
    KF${KF_MAJOR_VERSION}::I18n
    KF${KF_MAJOR_VERSION}::Contacts
    KPim${KF_MAJOR_VERSION}::MailImporterAkonadi
    KF5::MailTransport
    KF5::IdentityManagement
    KF5::MailCommon
    )
