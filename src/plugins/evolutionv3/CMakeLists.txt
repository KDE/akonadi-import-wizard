set(evolutionv3importer_SRCS
    evolutionaddressbook.cpp
    evolutionv3importdata.cpp
    evolutionsettings.cpp
    evolutioncalendar.cpp
    evolutionutil.cpp
    )
ecm_qt_declare_logging_category(evolutionv3importer_SRCS HEADER evolutionv3plugin_debug.h
        IDENTIFIER EVOLUTIONPLUGIN_LOG CATEGORY_NAME org.kde.pim.importwizard.evolutionplugin
        DESCRIPTION "importwizard (thunderbird plugin)"
        EXPORT IMPORTWIZARD
    )


kcoreaddons_add_plugin(evolutionv3importerplugin SOURCES ${evolutionv3importer_SRCS} INSTALL_NAMESPACE importwizard)
if (COMPILE_WITH_UNITY_CMAKE_SUPPORT)
    set_target_properties(evolutionv3importerplugin PROPERTIES UNITY_BUILD ON)
endif()

set_target_properties(evolutionv3importerplugin PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/importwizard/")

target_link_libraries(evolutionv3importerplugin
    KPim::ImportWizard
    KF5::XmlGui
    KF5::I18n
    KF5::Contacts
    KF5::MailImporterAkonadi
    KF5::MailTransport
    KF5::MailCommon
    KF5::IdentityManagement
    KF5::MessageViewer
    )
