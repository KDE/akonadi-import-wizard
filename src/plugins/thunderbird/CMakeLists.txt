set(thunderbirdimporter_SRCS
    thunderbirdaddressbook.cpp
    thunderbirdimportdata.cpp
    thunderbirdsettings.cpp
    addressbook/MorkParser.cpp
    thunderbirdaddressbook.h
    thunderbirdimportdata.h
    addressbook/MorkParser.h
    thunderbirdsettings.h
    )
ecm_qt_declare_logging_category(thunderbirdimporter_SRCS HEADER thunderbirdplugin_debug.h
        IDENTIFIER THUNDERBIRDPLUGIN_LOG CATEGORY_NAME org.kde.pim.importwizard.thunderbirdplugin
        DESCRIPTION "importwizard (thunderbird plugin)"
        EXPORT IMPORTWIZARD
    )

kcoreaddons_add_plugin(thunderbirdimporterplugin SOURCES ${thunderbirdimporter_SRCS} INSTALL_NAMESPACE pim${QT_MAJOR_VERSION}/importwizard)
set_target_properties(thunderbirdimporterplugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/importwizard/")
if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(thunderbirdimporterplugin PROPERTIES UNITY_BUILD ON)
endif()

target_link_libraries(thunderbirdimporterplugin
    KPim::ImportWizard
    KF5::XmlGui
    KF5::I18n
    KF5::Contacts
    KF5::MailImporterAkonadi
    KF5::MailTransport
    KF5::IdentityManagement
    KF5::MailCommon
    )
