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

kcoreaddons_add_plugin(seamonkeyimporterplugin SOURCES ${seamonkeyimporter_SRCS} INSTALL_NAMESPACE pim6/importwizard)
set_target_properties(seamonkeyimporterplugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/importwizard/")
if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(seamonkeyimporterplugin PROPERTIES UNITY_BUILD ON)
endif()

target_link_libraries(seamonkeyimporterplugin
    KPim6::ImportWizard
    KF6::XmlGui
    KF6::I18n
    KF6::Contacts
    KPim6::MailImporterAkonadi
    KPim6::MailTransport
    KPim6::IdentityManagement
    KPim6::MailCommon
    )
